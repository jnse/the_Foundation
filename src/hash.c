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

iDeclareType(HashNode);

#define iHashMaxElements    8 // per node
#define iHashMaxDepth       32

#define iHashNodeChildCount 4
#define iHashNodeChildMask  0x3
#define iHashNodeChildShift 2
#define iHashNodeLastChild  (iHashNodeChildCount - 1)

struct Impl_HashNode {
    iHashNode *parent;
    iHashNode *child[iHashNodeChildCount];
    iHashElement *element;
};

#define isBranchNode_HashNode_(d)       ((d)->child[0] != NULL)
#define shiftKey_HashNode_(key, levels) ((key) >> (iHashNodeChildShift * (levels)))
#define childNode_HashNode_(d, key, depth) \
        ((d)->child[shiftKey_HashNode_(key, depth) & iHashNodeChildMask])

static void delete_HashNode_(iHashNode *d) {
    for (int i = 0; i < iHashNodeChildCount; ++i) {
        if (d->child[i]) {
            delete_HashNode_(d->child[i]);
        }
    }
    free(d);
}

static iHashNode *findNode_HashNode_(iHashNode *d, iHashKey key, int *depth) {
    *depth = 0;
    while (isBranchNode_HashNode_(d)) {
        d = d->child[key & iHashNodeChildMask];
        key = shiftKey_HashNode_(key, 1);
        *depth += 1;
    }
    return d;
}

static iHashElement *findElement_HashNode_(const iHashNode *d, iHashKey key) {
    const iHashKey elemKey = key;
    while (isBranchNode_HashNode_(d)) {
        d = d->child[key & iHashNodeChildMask];
        key = shiftKey_HashNode_(key, 1);
    }
    for (iHashElement *i = d->element; i; i = i->next) {
        if (i->key == elemKey) {
            return i;
        }
    }
    return NULL;
}

static void addElement_HashNode_(iHashNode *d, iHashElement *element) {
    element->next = d->element;
    d->element = element;
}

static void split_HashNode_(iHashNode *d, int depth) {
    iAssert(depth < iHashMaxDepth);
    // Create the new children.
    for (int i = 0; i < iHashNodeChildCount; ++i) {
        d->child[i] = calloc(sizeof(iHashNode), 1);
        d->child[i]->parent = d;
    }
    // Divide the listed elements.
    iHashElement *next;
    for (iHashElement *i = d->element; i; i = next) {
        next = i->next;
        iHashNode *dest = childNode_HashNode_(d, i->key, depth);
        addElement_HashNode_(dest, i);
    }
    d->element = NULL;
}

static iBool isEmpty_HashNode_(const iHashNode *d) {
    return d->element == NULL && d->child[0] == NULL && d->child[1] == NULL;
}

static iHashNode *trim_HashNode_(iHashNode *d) {
    while (d->parent &&
           isEmpty_HashNode_(d->parent->child[0]) &&
           isEmpty_HashNode_(d->parent->child[1]) &&
           isEmpty_HashNode_(d->parent->child[2]) &&
           isEmpty_HashNode_(d->parent->child[3])) {
        d = d->parent;
        // Both child nodes empty, get rid of them.
        for (int i = 0; i < iHashNodeChildCount; ++i) {
            free(d->child[i]);
            d->child[i] = NULL;
        }
    }
    return d;
}

static iHashElement *remove_HashNode_(iHashNode **d, iHashKey key) {
    for (iHashElement *i = (*d)->element, **prev = &(*d)->element; i; i = i->next) {
        if (i->key == key) {
            *prev = i->next;
            i->next = NULL;
            if ((*d)->element == NULL) {
                // Node became empty, it may need removing.
                *d = trim_HashNode_(*d);
            }
            return i;
        }
        prev = &i->next;
    }
    return NULL;
}

//---------------------------------------------------------------------------------------

iDefineTypeConstruction(Hash)

void init_Hash(iHash *d) {
    d->root = calloc(sizeof(iHashNode), 1);
    d->size = 0;
}

void deinit_Hash(iHash *d) {
    delete_HashNode_(d->root);
}

iBool contains_Hash(const iHash *d, iHashKey key) {
    return value_Hash(d, key) == NULL;
}

iHashElement *value_Hash(const iHash *d, iHashKey key) {
    return findElement_HashNode_(d->root, key);
}

void clear_Hash(iHash *d) {
    delete_HashNode_(d->root);
    iZap(*d->root);
    d->size = 0;
}

iHashElement *insert_Hash(iHash *d, iHashElement *element) {
    iAssert(element != NULL);
    int depth;
    iHashNode *node = findNode_HashNode_(d->root, element->key, &depth);
    iHashElement *existing = NULL;
    size_t nodeSize = 0;
    // An existing element with a clashing key must be removed.
    for (iHashElement *i = node->element, **prev = &node->element; i; i = i->next, nodeSize++) {
        if (i->key == element->key) {
            *prev = i->next;
            existing = i;
            existing->next = NULL;
            d->size--;
            break;
        }
        prev = &i->next;
    }
    if (nodeSize >= iHashMaxElements) {
        split_HashNode_(node, depth);
        addElement_HashNode_(childNode_HashNode_(node, element->key, depth), element);
    }
    else {
        addElement_HashNode_(node, element);
    }
    // Update total count.
    d->size++;
    return existing;
}

iHashElement *remove_Hash(iHash *d, iHashKey key) {
    int depth;
    iHashNode *node = findNode_HashNode_(d->root, key, &depth);
    iHashElement *removed = remove_HashNode_(&node, key);
    if (removed) d->size--;
    return removed;
}

//---------------------------------------------------------------------------------------

iHashNode *firstNodePreOrder_HashNode_(const iHashNode *d) {
    if (!d) return NULL;
    if (d->element) {
        iAssert(d->child[0] == NULL);
        iAssert(d->child[1] == NULL);
        iAssert(d->child[2] == NULL);
        iAssert(d->child[3] == NULL);
        return iConstCast(iHashNode *, d);
    }
    iHashNode *elem = NULL;
    for (int i = 0; i < iHashNodeChildCount; ++i) {
        if ((elem = firstNodePreOrder_HashNode_(d->child[i])) != NULL) {
            break;
        }
    }
    return elem;
}

static int ordinal_HashNode_(const iHashNode *d) {
    if (d->parent) {
        for (int i = 0; i < iHashNodeChildCount; ++i) {
            if (d->parent->child[i] == d) return i;
        }
    }
    return -1;
}

iHashNode *nextNodePreOrder_HashNode_(const iHashNode *d) {
    iAssert(d);
    // Nothing follows the root.
    if (!d->parent) return NULL;
    // Switch to the next sibling.
    for (int i = 0; i < iHashNodeLastChild; ++i) {
        if (d->parent->child[i] == d) {
            return firstNodePreOrder_HashNode_(d->parent->child[i + 1]);
        }
    }
    // Go back up until there's a node on the right.
    int ord = iHashNodeLastChild;
    for (; ord == iHashNodeLastChild; d = d->parent) {
        if (!d->parent) return NULL;
        ord = ordinal_HashNode_(d);
    }
    return firstNodePreOrder_HashNode_(d->child[ord + 1]);
}

void init_HashIterator(iHashIterator *d, iHash *hash) {
    d->hash = hash;
    d->node = firstNodePreOrder_HashNode_(hash->root);
    d->value = (d->node? d->node->element : NULL);
    // The current element may be deleted, so keep the next one in a safe place.
    d->next = (d->value? d->value->next : NULL);
}

void next_HashIterator(iHashIterator *d) {
    d->value = d->next;
    if (!d->value) {
        if((d->node = nextNodePreOrder_HashNode_(d->node)) != NULL) {
            d->value = d->node->element;
        }
    }
    d->next = (d->value? d->value->next : NULL);
}

iHashElement *remove_HashIterator(iHashIterator *d) {
    remove_HashNode_(&d->node, d->value->key);
    d->hash->size--;
    return d->value;
}

void init_HashConstIterator(iHashConstIterator *d, const iHash *hash) {
    d->hash = hash;
    d->node = firstNodePreOrder_HashNode_(hash->root);
    d->value = (d->node? d->node->element : NULL);
}

void next_HashConstIterator(iHashConstIterator *d) {
    d->value = d->value->next;
    if (!d->value) {
        if((d->node = nextNodePreOrder_HashNode_(d->node)) != NULL) {
            d->value = d->node->element;
        }
    }
}
