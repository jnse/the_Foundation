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

iDefineTypeConstructionArgs(Map, (iMapElementCmpFunc cmp), cmp)

enum iMapElementFlag {
    red_MapElementFlag = 0,
    black_MapElementFlag = 1,
};

enum iRotation {
    left_Rotation = 0,
    right_Rotation = 1,
};

#define grandParent_MapElement_(d) ((d)->parent? (d)->parent->parent : NULL)

static iMapElement *sibling_MapElement_(iMapElement *d) {
    if (!d->parent) return NULL;
    return d->parent->child[d->parent->child[0] == d? 1 : 0];
}

static iMapElement *uncle_MapElement_(iMapElement *d) {
    if (!grandParent_MapElement_(d)) return NULL;
    return sibling_MapElement_(d->parent);
}

//static void replace_MapElement_(iMapElement *d, iMapElement *other) {
//    // Replace this element.
//    if (d->parent) {
//        d->parent->child[d->parent->child[0] == d? 0 : 1] = other;
//    }
//}

static int childIndex_MapElement_(const iMapElement *d) {
    iAssert(d->parent);
    return d->parent->child[0] == d? 0 : 1;
}

static iMapElement **parentLink_MapElement_(iMapElement *d) {
    if (!d->parent) return NULL;
    return d->parent->child + (d->parent->child[0] == d? 0 : 1);
}

static void rotate_MapElement_(iMapElement *d, enum iRotation dir) {
    const int side = (dir == left_Rotation? 1 : 0);
    iMapElement **oldParentLink = parentLink_MapElement_(d);
    iMapElement *newD = d->child[side];
    iAssert(newD != NULL);
    d->child[side] = newD->child[side ^ 1];
    d->child[side]->parent = d;
    newD->child[side ^ 1] = d;
    newD->parent = d->parent;
    if (oldParentLink) *oldParentLink = newD;
    d->parent = newD;
}

static void setChild_MapElement_(iMapElement *d, int index, iMapElement *child) {
    d->child[index] = child;
    if (child) {
        child->parent = d;
    }
}

static iMapElement *insertElement_Map_(iMap *d, iMapElement *insert) {
    iMapElement *root = d->root;
    while (root) {
        const int cmp = d->cmp(root->key, insert->key);
        if (cmp == 0) {
            // Replace this element; keys must be unique.
            iMapElement **parentLink = parentLink_MapElement_(root);
            if (parentLink) *parentLink = insert;
            insert->parent = root->parent;
            insert->flags = root->flags;
            for (int i = 0; i < 2; ++i) {
                setChild_MapElement_(insert, i, root->child[i]);
            }
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
    insert->flags = red_MapElementFlag;
    return NULL; // New element added.
}

static void repairAfterInsert_MapElement_(iMapElement *elem) {
    // Restore the required red/black properties.
    while (elem) {
        if (!elem->parent) {
            elem->flags = black_MapElementFlag;
            break;
        }
        else if (elem->parent->flags == black_MapElementFlag) {
            break;
        }
        else if (uncle_MapElement_(elem)->flags == red_MapElementFlag) {
            elem->parent->flags = black_MapElementFlag;
            uncle_MapElement_(elem)->flags = black_MapElementFlag;
            grandParent_MapElement_(elem)->flags = red_MapElementFlag;
            elem = grandParent_MapElement_(elem);
        }
        else {
            {
                iMapElement *p = elem->parent;
                iMapElement *g = grandParent_MapElement_(elem);
                if (elem == g->child[0]->child[1]) {
                    rotate_MapElement_(p, left_Rotation);
                    elem = elem->child[0];
                }
                else if (elem == g->child[1]->child[0]) {
                    rotate_MapElement_(p, right_Rotation);
                    elem = elem->child[1];
                }
            }
            {
                iMapElement *p = elem->parent;
                iMapElement *g = grandParent_MapElement_(elem);
                rotate_MapElement_(g, elem == p->child[0]? right_Rotation : left_Rotation);
                p->flags = black_MapElementFlag;
                g->flags = red_MapElementFlag;
            }
            break;
        }
    }
}

void init_Map(iMap *d, iMapElementCmpFunc cmp) {
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

iMapElement *value_Map(const iMap *d, iMapKey key) {
    iMapElement *elem = d->root;
    while (elem) {
        const int cmp = d->cmp(elem->key, key);
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

iMapElement *insert_Map(iMap *d, iMapElement *element) {
    iMapElement *old = insertElement_Map_(d, element);
    if (old) {
        // The root may have been replaced.
        if (d->root == old) d->root = element;
        return old;
    }
    repairAfterInsert_MapElement_(element);
    // Re-root.
    iMapElement *newRoot = element;
    while (newRoot->parent) {
        newRoot = newRoot->parent;
    }
    d->root = newRoot;
    return NULL; // no previous element replaced
}

iMapElement *remove_Map(iMap *d, iMapKey key) {

}
