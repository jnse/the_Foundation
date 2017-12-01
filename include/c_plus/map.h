#pragma once

/** @file c_plus/map.h  Map of sorted unique integer elements.

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

#include "defs.h"
#include "list.h"
#include "class.h"

iDeclareType(Map)
iDeclareType(MapNode)
iDeclareType(Block)

typedef intptr_t iMapKey;
typedef int (*iMapNodeCmpFunc)(iMapKey, iMapKey);

struct Impl_Map {
    size_t size;
    iMapNode *root;
    iMapNodeCmpFunc cmp;
};

/// Elements inserted to the map must be based on iMapNode.
struct Impl_MapNode {
    iMapNode *parent;
    iMapNode *child[2];
    int flags;
    iMapKey key;
};

typedef void iAnyElement;

/**
 * Constructs a new map.
 *
 * Map does not have ownership of the elements. This means the elements can be
 * any type of object as long as they are derived from MapNode.
 *
 * @return Map instance.
 */
iMap *      new_Map     (iMapNodeCmpFunc cmp);

void        delete_Map  (iMap *);

#define     collect_Map(d)  iCollectDel(d, delete_Map)

void        init_Map    (iMap *, iMapNodeCmpFunc cmp);
void        deinit_Map  (iMap *);

#define     size_Map(d)     ((d)->size)
#define     isEmpty_Map(d)  (size_Map(d) == 0)

iBool       contains_Map    (const iMap *, iMapKey key);
iMapNode *  value_Map       (const iMap *, iMapKey key);

void        clear_Map   (iMap *);

/**
 * Inserts an element into the map.
 *
 * @param element  Element to be inserted. Ownership not taken. The `key` member must
 *                 be set to the key value.
 *
 * @return Previous element with the same key that had to be removed from the hash to
 * make room for the new element. The caller should delete the element or take any other
 * necessary actions, since it is no longer part of the hash.
 */
iMapNode *  insert_Map (iMap *, iMapNode *node);

iMapNode *  remove_Map      (iMap *, iMapKey key);
iMapNode *  removeNode_Map  (iMap *, iMapNode *node);

iDeclareIterator(Map, iMap *)
iMapNode *remove_MapIterator(iMapIterator *d);
struct IteratorImpl_Map {
    iMapNode *value;
    iMapNode *next;
    //int dir;
    iMap *map;
};

iDeclareConstIterator(Map, const iMap *)
struct ConstIteratorImpl_Map {
    const iMapNode *value;
    const iMap *map;
};
