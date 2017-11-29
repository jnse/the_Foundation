/** @file hash.c  Hash of integer values.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

<small>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</small>
*/

#include "c_plus/hash.h"
#include "c_plus/set.h"

#include <stdlib.h>

iDeclareType(HashBucket)

#define iHashMaxNodes           8 // per node
#define iHashMaxDepth           32

#define iHashBucketChildCount   4
#define iHashBucketChildMask    0x3
#define iHashBucketChildShift   2
#define iHashBucketLastChild    (iHashBucketChildCount - 1)

struct Impl_HashBucket {
    iHashBucket *parent;
    iHashBucket *child[iHashBucketChildCount];
    iHashNode *element;
};

#define isBranch_HashBucket_(d)             ((d)->child[0] != NULL)
#define shiftKey_HashBucket_(key, levels)   ((key) >> (iHashBucketChildShift * (levels)))
#define child_HashBucket_(d, key, depth)    \
        ((d)->child[shiftKey_HashBucket_(key, depth) & iHashBucketChildMask])

static void delete_HashBucket_(iHashBucket *d) {
    if (d) {
        for (int i = 0; i < iHashBucketChildCount; ++i) {
            if (d->child[i]) {
                delete_HashBucket_(d->child[i]);
            }
        }
        free(d);
    }
}

static iHashBucket *find_HashBucket_(iHashBucket *d, iHashKey key, int *depth) {
    *depth = 0;
    while (isBranch_HashBucket_(d)) {
        d = d->child[key & iHashBucketChildMask];
        key = shiftKey_HashBucket_(key, 1);
        *depth += 1;
    }
    return d;
}

static iHashNode *findNode_HashBucket_(const iHashBucket *d, iHashKey key) {
    const iHashKey elemKey = key;
    while (isBranch_HashBucket_(d)) {
        d = d->child[key & iHashBucketChildMask];
        key = shiftKey_HashBucket_(key, 1);
    }
    for (iHashNode *i = d->element; i; i = i->next) {
        if (i->key == elemKey) {
            return i;
        }
    }
    return NULL;
}

static void addNode_HashBucket_(iHashBucket *d, iHashNode *element) {
    element->next = d->element;
    d->element = element;
}

static void split_HashBucket_(iHashBucket *d, int depth) {
    iAssert(depth < iHashMaxDepth);
    // Create the new children.
    for (int i = 0; i < iHashBucketChildCount; ++i) {
        d->child[i] = calloc(sizeof(iHashBucket), 1);
        d->child[i]->parent = d;
    }
    // Divide the listed elements.
    iHashNode *next;
    for (iHashNode *i = d->element; i; i = next) {
        next = i->next;
        iHashBucket *dest = child_HashBucket_(d, i->key, depth);
        addNode_HashBucket_(dest, i);
    }
    d->element = NULL;
}

static iBool isEmpty_HashBucket_(const iHashBucket *d) {
    return d->element == NULL && d->child[0] == NULL && d->child[1] == NULL;
}

static iHashBucket *trim_HashBucket_(iHashBucket *d) {
    while (d->parent &&
           isEmpty_HashBucket_(d->parent->child[0]) &&
           isEmpty_HashBucket_(d->parent->child[1]) &&
           isEmpty_HashBucket_(d->parent->child[2]) &&
           isEmpty_HashBucket_(d->parent->child[3])) {
        d = d->parent;
        // Both child nodes empty, get rid of them.
        for (int i = 0; i < iHashBucketChildCount; ++i) {
            free(d->child[i]);
            d->child[i] = NULL;
        }
    }
    return d;
}

static iHashNode *remove_HashBucket_(iHashBucket **d, iHashKey key) {
    for (iHashNode *i = (*d)->element, **prev = &(*d)->element; i; i = i->next) {
        if (i->key == key) {
            *prev = i->next;
            i->next = NULL;
            if ((*d)->element == NULL) {
                // Node became empty, it may need removing.
                *d = trim_HashBucket_(*d);
            }
            return i;
        }
        prev = &i->next;
    }
    return NULL;
}

static int ordinal_HashBucket_(const iHashBucket *d) {
    if (d->parent) {
        for (int i = 0; i < iHashBucketChildCount; ++i) {
            if (d->parent->child[i] == d) return i;
        }
    }
    return -1;
}

static iHashBucket *firstInOrder_HashBucket_(const iHashBucket *d) {
    if (!d) return NULL;
    if (d->element) {
        iAssert(d->child[0] == NULL);
        iAssert(d->child[1] == NULL);
        iAssert(d->child[2] == NULL);
        iAssert(d->child[3] == NULL);
        return iConstCast(iHashBucket *, d);
    }
    iHashBucket *elem = NULL;
    for (int i = 0; i < iHashBucketChildCount; ++i) {
        if ((elem = firstInOrder_HashBucket_(d->child[i])) != NULL) {
            break;
        }
    }
    return elem;
}

static iHashBucket *nextInOrder_HashBucket_(const iHashBucket *d) {
    iAssert(d);
    // Nothing follows the root.
    if (!d->parent) return NULL;
    // Switch to the next sibling.
    for (int i = 0; i < iHashBucketLastChild; ++i) {
        if (d->parent->child[i] == d) {
            return firstInOrder_HashBucket_(d->parent->child[i + 1]);
        }
    }
    // Go back up until there's a node on the right.
    int ord = iHashBucketLastChild;
    for (; ord == iHashBucketLastChild; d = d->parent) {
        if (!d->parent) return NULL;
        ord = ordinal_HashBucket_(d);
    }
    return firstInOrder_HashBucket_(d->child[ord + 1]);
}

//---------------------------------------------------------------------------------------

iDefineTypeConstruction(Hash)

void init_Hash(iHash *d) {
    d->root = calloc(sizeof(iHashBucket), 1);
    d->size = 0;
}

void deinit_Hash(iHash *d) {
    delete_HashBucket_(d->root);
}

iBool contains_Hash(const iHash *d, iHashKey key) {
    return value_Hash(d, key) == NULL;
}

iHashNode *value_Hash(const iHash *d, iHashKey key) {
    return findNode_HashBucket_(d->root, key);
}

void clear_Hash(iHash *d) {
    for (int i = 0; i < iHashBucketChildCount; ++i) {
        delete_HashBucket_(d->root->child[i]);
    }
    iZap(*d->root);
    d->size = 0;
}

iHashNode *insert_Hash(iHash *d, iHashNode *element) {
    iAssert(element != NULL);
    int depth;
    iHashBucket *node = find_HashBucket_(d->root, element->key, &depth);
    iHashNode *existing = NULL;
    size_t nodeSize = 0;
    // An existing element with a clashing key must be removed.
    for (iHashNode *i = node->element, **prev = &node->element; i; i = i->next, nodeSize++) {
        if (i->key == element->key) {
            *prev = i->next;
            existing = i;
            existing->next = NULL;
            d->size--;
            break;
        }
        prev = &i->next;
    }
    if (nodeSize >= iHashMaxNodes) {
        split_HashBucket_(node, depth);
        addNode_HashBucket_(child_HashBucket_(node, element->key, depth), element);
    }
    else {
        addNode_HashBucket_(node, element);
    }
    // Update total count.
    d->size++;
    return existing;
}

iHashNode *remove_Hash(iHash *d, iHashKey key) {
    int depth;
    iHashBucket *node = find_HashBucket_(d->root, key, &depth);
    iHashNode *removed = remove_HashBucket_(&node, key);
    if (removed) d->size--;
    return removed;
}

//---------------------------------------------------------------------------------------

void init_HashIterator(iHashIterator *d, iHash *hash) {
    d->hash = hash;
    d->node = firstInOrder_HashBucket_(hash->root);
    d->value = (d->node? d->node->element : NULL);
    // The current element may be deleted, so keep the next one in a safe place.
    d->next = (d->value? d->value->next : NULL);
}

void next_HashIterator(iHashIterator *d) {
    d->value = d->next;
    if (!d->value) {
        if((d->node = nextInOrder_HashBucket_(d->node)) != NULL) {
            d->value = d->node->element;
        }
    }
    d->next = (d->value? d->value->next : NULL);
}

iHashNode *remove_HashIterator(iHashIterator *d) {
    remove_HashBucket_(&d->node, d->value->key);
    d->hash->size--;
    return d->value;
}

void init_HashConstIterator(iHashConstIterator *d, const iHash *hash) {
    d->hash = hash;
    d->node = firstInOrder_HashBucket_(hash->root);
    d->value = (d->node? d->node->element : NULL);
}

void next_HashConstIterator(iHashConstIterator *d) {
    d->value = d->value->next;
    if (!d->value) {
        if((d->node = nextInOrder_HashBucket_(d->node)) != NULL) {
            d->value = d->node->element;
        }
    }
}
