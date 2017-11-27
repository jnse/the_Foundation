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
    iPtrHashElement base;
    iBlock keyBlock;
    iAnyObject *object;
};

iBlockHashElement *     new_BlockHashElement(const iBlock *key, iAnyObject *object);
void                    delete_BlockHashElement(iBlockHashElement *);

#define         key_BlockHashElement(d)    iConstCast(iBlock *, (&((const iBlockHashElement *) (d))->keyBlock))

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

//---------------------------------------------------------------------------------------
// Deriving specialized hashes:

#define iDeclareBlockHash(typeName, keyType, valueObject) \
    typedef iBlockHash typeName; \
    typedef keyType typeName##Key; \
    typedef iBlockHashElement typeName##Element; \
    \
    typeName##Element * new_##typeName##Element     (const keyType *key, valueObject *object); \
    void                delete_##typeName##Element  (typeName##Element *); \
    void                key_##typeName##Element     (const typeName##Element *, keyType *key); \
    valueObject *       value_##typeName##Element   (const typeName##Element *); \
    \
    typeName *          new_##typeName      (void); \
    void                delete_##typeName   (typeName *); \
    typeName *          collect_##typeName  (typeName *); \
    \
    void                init_##typeName     (typeName *); \
    void                deinit_##typeName   (typeName *); \
    \
    size_t              size_##typeName     (const typeName *); \
    iBool               isEmpty_##typeName  (const typeName *); \
    \
    iBool               contains_##typeName     (const typeName *, const keyType *key); \
    const valueObject * constValue_##typeName   (const typeName *, const keyType *key); \
    valueObject *       value_##typeName        (typeName *, const keyType *key); \
    \
    void                clear_##typeName    (typeName *); \
    \
    iBool               insert_##typeName   (typeName *, const keyType *key, valueObject *value); \
    iBool               remove_##typeName   (typeName *, const keyType *key);

/**
 * Methods that must be manually defined:
 * - key_<typeName>Element(element, key): copies the Block back to an existing keyType instance
 * - initBlock_<typeName>Key(key, block): initializes a Block with the key data
 */
#define iDefineBlockHash(typeName, keyType, valueObject) \
    typeName##Element *new_##typeName##Element(const keyType *key, valueObject *object) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iAnyElement *elem = new_BlockHashElement(&bkey, object); \
        deinit_Block(&bkey); \
        return elem; \
    } \
    void delete_##typeName##Element(typeName##Element *d) { delete_BlockHashElement(d); } \
    typeName *new_##typeName(void) { \
        typeName *d = malloc(sizeof(typeName)); \
        init_##typeName(d); return d; \
    } \
    valueObject *value_##typeName##Element(const typeName##Element *d) {\
        return (valueObject *) d->object; \
    } \
    void delete_##typeName(typeName *d) { \
        deinit_##typeName(d); free(d); \
    } \
    void init_##typeName(typeName *d) { \
        init_BlockHash(d); \
    } \
    void deinit_##typeName(typeName *d) { \
        deinit_BlockHash(d); \
    } \
    typeName *collect_##typeName(typeName *d) { return iCollectDel(d, delete_##typeName); } \
    size_t size_##typeName(const typeName *d) { return size_BlockHash(d); } \
    size_t isEmpty_##typeName(const typeName *d) { return isEmpty_BlockHash(d); } \
    void clear_##typeName(typeName *d) { clear_BlockHash(d); } \
    iBool contains_##typeName(const typeName *d, const keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = contains_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return res; \
    } \
    const valueObject *constValue_##typeName(const typeName *d, const keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        const valueObject *obj = (const valueObject *) constValue_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return obj; \
    } \
    valueObject *value_##typeName(typeName *d, const keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        valueObject *obj = (valueObject *) value_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return obj; \
    } \
    iBool insert_##typeName(typeName *d, const keyType *key, valueObject *value) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = insertKey_BlockHash(d, &bkey)); \
        deinit_Block(&bkey); \
        return res; \
    } \
    iBool remove_##typeName(typeName *d, const keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = remove_BlockHash(d, &bkey); \
        deinit_Block(bkey); \
        return res; \
    } \
