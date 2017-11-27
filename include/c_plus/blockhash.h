#pragma once

/** @file c_plus/blockhash.h  Hash that uses Block for keys and Object for values.

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

#include "ptrhash.h"
#include "string.h"
#include "object.h"

iDeclareType(BlockHashElement);

typedef iPtrHash iBlockHash;

struct Impl_BlockHashElement {
    iPtrHashElement base; // key Block held here
    iAnyObject *object;
};

iBlockHashElement *     new_BlockHashElement(iBlock *key, iAnyObject *object);
void                    delete_BlockHashElement(iBlockHashElement *);

#define         key_BlockHashElement(d)    (iConstCast(iBlock *, (d)->base.key))

iBlockHash *    new_BlockHash     (void);
void            delete_BlockHash  (iBlockHash *);

#define         collect_BlockHash(d)       iCollectDel(d, delete_BlockHash)

void            init_BlockHash    (iBlockHash *);
void            deinit_BlockHash  (iBlockHash *);

#define         size_BlockHash(d)          size_Hash(&(d)->hash)
#define         isEmpty_BlockHash(d)       isEmpty_Hash(&(d)->hash)

iBool               contains_BlockHash    (const iBlockHash *, const iBlock *key);
const iAnyObject *  constValue_BlockHash  (const iBlockHash *, const iBlock *key);
iAnyObject *        value_BlockHash       (iBlockHash *, const iBlock *key);

void            clear_BlockHash        (iBlockHash *);

iBool           insert_BlockHash       (iBlockHash *, const iBlock *key, iAnyObject *value);

/**
 * Insert a key-value element into the BlockHash.
 *
 * @param key    Key string. Ownership taken.
 * @param value  Value object.
 *
 * @return @c iTrue, if the a new key-value element was added and the size of the hash
 * increased as a result. @c False, if an existing one was replaced.
 */
iBool           insertKey_BlockHash    (iBlockHash *, iBlock *key, iAnyObject *value);

iBool           remove_BlockHash       (iBlockHash *, const iBlock *key);

void            insertValues_BlockHash       (iBlockHash *, const iBlock *key, iAnyObject *value, ...);
void            insertValuesCStr_BlockHash   (iBlockHash *, const char *key, iAnyObject *value, ...);

iDeclareIterator(BlockHash, iBlockHash *);
const iBlock *  key_BlockHashIterator(iBlockHashIterator *);
void            remove_BlockHashIterator(iBlockHashIterator *);
struct IteratorImpl_BlockHash {
    iHashIterator base;
    iBlockHashElement *value;
};

iDeclareConstIterator(BlockHash, const iBlockHash *);
const iBlock *  key_BlockHashConstIterator(iBlockHashConstIterator *);
struct ConstIteratorImpl_BlockHash {
    iHashConstIterator base;
    const iBlockHashElement *value;
};
