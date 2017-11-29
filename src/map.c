/** @file map.c  Map of sorted unique integer keys.

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

#include "c_plus/map.h"

#include <stdlib.h>

iDefineTypeConstructionArgs(Map, (iMapNodeCmpFunc cmp), cmp)

enum iMapNodeFlag {
    red_MapNodeFlag = 0,
    black_MapNodeFlag = 1,
};

enum iRotation {
    left_Rotation = 0,
    right_Rotation = 1,
};

#define grandParent_MapNode_(d) ((d)->parent? (d)->parent->parent : NULL)

static iMapNode *sibling_MapNode_(iMapNode *d) {
    if (!d->parent) return NULL;
    return d->parent->child[d->parent->child[0] == d? 1 : 0];
}

static iMapNode *uncle_MapNode_(iMapNode *d) {
    if (!grandParent_MapNode_(d)) return NULL;
    return sibling_MapNode_(d->parent);
}

//static void replace_MapNode_(iMapNode *d, iMapNode *other) {
//    // Replace this element.
//    if (d->parent) {
//        d->parent->child[d->parent->child[0] == d? 0 : 1] = other;
//    }
//}

//static int childIndex_MapNode_(const iMapNode *d) {
//    iAssert(d->parent);
//    return d->parent->child[0] == d? 0 : 1;
//}

static iMapNode **parentLink_MapNode_(iMapNode *d) {
    if (!d->parent) return NULL;
    return d->parent->child + (d->parent->child[0] == d? 0 : 1);
}

static void rotate_MapNode_(iMapNode *d, enum iRotation dir) {
    const int side = (dir == left_Rotation? 1 : 0);
    iMapNode **oldParentLink = parentLink_MapNode_(d);
    iMapNode *newD = d->child[side];
    iAssert(newD != NULL);
    d->child[side] = newD->child[side ^ 1];
    if (d->child[side]) d->child[side]->parent = d;
    newD->child[side ^ 1] = d;
    newD->parent = d->parent;
    if (oldParentLink) *oldParentLink = newD;
    d->parent = newD;
}

static void setChild_MapNode_(iMapNode *d, int index, iMapNode *child) {
    d->child[index] = child;
    if (child) {
        child->parent = d;
    }
}

static void replaceElement_MapNode_(iMapNode *d, iMapNode *replacement) {
    iMapNode **parentLink = parentLink_MapNode_(d);
    if (parentLink) *parentLink = replacement;
    replacement->parent = d->parent;
    for (int i = 0; i < 2; ++i) {
        setChild_MapNode_(replacement, i, d->child[i]);
    }
}

static iMapNode *insertElement_Map_(iMap *d, iMapNode *insert) {
    iMapNode *root = d->root;
    while (root) {
        const int cmp = d->cmp(insert->key, root->key);
        if (cmp == 0) {
            // Replace this element; keys must be unique.
            replaceElement_MapNode_(root, insert);
            insert->flags = root->flags;
            return root; // The old element.
        }
        const int side = (cmp < 0? 0 : 1);
        if (root->child[side]) {
            root = root->child[side];
        }
        else {
            root->child[side] = insert;
            break;
        }
    }
    insert->parent = root;
    insert->child[0] = insert->child[1] = NULL;
    insert->flags = red_MapNodeFlag;
    return NULL; // New element added.
}

static void repairAfterInsert_MapNode_(iMapNode *elem) {
    // Restore the required red/black properties.
    while (elem) {
        if (!elem->parent) {
            elem->flags = black_MapNodeFlag;
            break;
        }
        else if (elem->parent->flags == black_MapNodeFlag) {
            break;
        }
        else if (uncle_MapNode_(elem) &&
                 uncle_MapNode_(elem)->flags == red_MapNodeFlag) {
            elem->parent->flags = black_MapNodeFlag;
            uncle_MapNode_(elem)->flags = black_MapNodeFlag;
            grandParent_MapNode_(elem)->flags = red_MapNodeFlag;
            elem = grandParent_MapNode_(elem);
        }
        else {
            iMapNode *p = elem->parent;
            iMapNode *g = grandParent_MapNode_(elem);
            if (g->child[0] && elem == g->child[0]->child[1]) {
                rotate_MapNode_(p, left_Rotation);
                elem = elem->child[0];
            }
            else if (g->child[1] && elem == g->child[1]->child[0]) {
                rotate_MapNode_(p, right_Rotation);
                elem = elem->child[1];
            }
            p = elem->parent;
            g = grandParent_MapNode_(elem);
            rotate_MapNode_(g, elem == p->child[0]? right_Rotation : left_Rotation);
            p->flags = black_MapNodeFlag;
            g->flags = red_MapNodeFlag;
            break;
        }
    }
}

void init_Map(iMap *d, iMapNodeCmpFunc cmp) {
    d->size = 0;
    d->root = NULL;
    d->cmp = cmp;
}

void deinit_Map(iMap *d) {
    iUnused(d);
}

iBool contains_Map(const iMap *d, iMapKey key) {
    return value_Map(d, key) != NULL;
}

iMapNode *value_Map(const iMap *d, iMapKey key) {
    iMapNode *elem = d->root;
    while (elem) {
        const int cmp = d->cmp(key, elem->key);
        if (cmp == 0) {
            return elem;
        }
        else {
            elem = elem->child[cmp > 0? 1 : 0];
        }
    }
    return elem;
}

void clear_Map(iMap *d) {
    d->size = 0;
    d->root = NULL;
}

iMapNode *insert_Map(iMap *d, iMapNode *element) {
    iMapNode *old = insertElement_Map_(d, element);
    if (old) {
        // The root may have been replaced.
        if (d->root == old) d->root = element;
        return old;
    }
    repairAfterInsert_MapNode_(element);
    // Re-root.
    iMapNode *newRoot = element;
    while (newRoot->parent) {
        newRoot = newRoot->parent;
    }
    d->root = newRoot;
    d->size++;
    return NULL; // no previous element replaced
}

iMapNode *remove_Map(iMap *d, iMapKey key) {
    return NULL;
}

//---------------------------------------------------------------------------------------

enum iMapIteratorDir {
    down_MapIteratorDir,
    up_MapIteratorDir,
};

static const iMapNode *firstInOrder_MapNode_(const iMapNode *d) {
    if (!d) return NULL;
    while (d->child[0]) {
        d = d->child[0];
    }
    return d;
}

static const iMapNode *nextInOrder_MapNode_(const iMapNode *d, int *dir) {
    if (!d) return NULL;
    // Switch to the next sibling.
    switch (*dir) {
        case down_MapIteratorDir:
            if (d->child[1]) {
                return firstInOrder_MapNode_(d->child[1]);
            }
            *dir = up_MapIteratorDir;
            if (!d->parent) return NULL;
            if (d->parent->child[0] == d) {
                return d->parent;
            }
            break;
        case up_MapIteratorDir:
            if (d->child[1]) {
                *dir = down_MapIteratorDir;
                return firstInOrder_MapNode_(d->child[1]);
            }
            break;
    }
    *dir = up_MapIteratorDir;
    // Go back up until there's a node on the right.
    for (;; d = d->parent) {
        if (!d->parent) return NULL; // Reached the root.
        if (d->parent->child[0] == d && d->parent->child[1]) return d->parent;
    }
}

void init_MapIterator(iMapIterator *d, iMap *map) {
    d->map = map;
    d->dir = down_MapIteratorDir;
    d->value = iConstCast(iMapNode *, firstInOrder_MapNode_(map->root));
    // The current element may be deleted, so keep the next one in a safe place.
    d->next = iConstCast(iMapNode *, nextInOrder_MapNode_(d->value, &d->dir));
}

void next_MapIterator(iMapIterator *d) {
    d->value = d->next;
    d->next = iConstCast(iMapNode *, nextInOrder_MapNode_(d->value, &d->dir));
}
