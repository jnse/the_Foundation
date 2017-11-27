#pragma once

/** @file c_plus/hash.h  Hash of integer values.

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
#include "ptrarray.h"
#include "list.h"
#include "class.h"

/**
 * Hash does not have ownership of the elements. This means the elements can be
 * any type of object as long as they are derived from HashElement.
 */
iDeclareType(Hash)
iDeclareType(HashElement)
iDeclareType(HashNode)

typedef uint32_t iHashKey;

struct Impl_Hash {
    iHashNode *root;
    size_t size;
};

/// Elements inserted to the hash must be based on iHashElement.
struct Impl_HashElement {
    iHashElement *next;
    iHashKey key;
};

typedef void iAnyElement;

iHash *         new_Hash    (void);
void            delete_Hash (iHash *);

#define         collect_Hash(d) iCollectDel(d, delete_Hash)

void            init_Hash   (iHash *);
void            deinit_Hash (iHash *);

#define         size_Hash(d)    ((d)->size)
#define         isEmpty_Hash(d) (size_Hash(d) == 0)

iBool           contains_Hash   (const iHash *, iHashKey key);
iHashElement *  value_Hash      (const iHash *, iHashKey key);

void            clear_Hash  (iHash *);

/**
 * Inserts an element into the hash.
 *
 * @param element  Element to be inserted. Ownership not taken. The `key` member must
 *                 be set to a valid hash key.
 *
 * @return Previous element with the same key that had to be removed from the hash to
 * make room for the new element. The caller should delete the element or take any other
 * necessary actions, since it is no longer part of the hash.
 */
iHashElement *  insert_Hash (iHash *, iHashElement *element);

iHashElement *  remove_Hash (iHash *, iHashKey key);

iDeclareIterator(Hash, iHash *)
iHashElement *remove_HashIterator(iHashIterator *d);
struct IteratorImpl_Hash {
    iHashElement *value;
    iHashElement *next;
    iHashNode *node;
    iHash *hash;
};

iDeclareConstIterator(Hash, const iHash *)
struct ConstIteratorImpl_Hash {
    const iHashElement *value;
    const iHashNode *node;
    const iHash *hash;
};
