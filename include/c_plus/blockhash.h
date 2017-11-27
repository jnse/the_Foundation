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
#include "block.h"
#include "object.h"

iDeclareType(BlockHash)
iDeclareType(BlockHashElement)

typedef iAnyElement *(*iBlockHashElementConstructor)(const iBlock *, const iAnyObject *);

iBeginDeclareClass(BlockHashElement)
    iBlockHashElement *(*new)(const iBlock *key, const iAnyObject *object);
iEndDeclareClass(BlockHashElement)

struct Impl_BlockHash {
    iPtrHash base;
    const iBlockHashElementClass *elementClass;
};

struct Impl_BlockHashElement {
    iPtrHashElement base;
    iBlock keyBlock;
    iAnyObject *object;
};

iBlockHashElement *     new_BlockHashElement(const iBlock *key, const iAnyObject *object);
void                    deinit_BlockHashElement(iBlockHashElement *);

#define         key_BlockHashElement(d)    iConstCast(iBlock *, (&((const iBlockHashElement *) (d))->keyBlock))

iBlockHash *    new_BlockHash     (void);
void            delete_BlockHash  (iBlockHash *);

#define         collect_BlockHash(d)       iCollectDel(d, delete_BlockHash)

void            init_BlockHash    (iBlockHash *);
void            deinit_BlockHash  (iBlockHash *);

void            setElementClass_BlockHash (iBlockHash *, const iBlockHashElementClass *class);

#define         size_BlockHash(d)          size_Hash(&(d)->base.hash)
#define         isEmpty_BlockHash(d)       isEmpty_Hash(&(d)->base.hash)

iBool               contains_BlockHash    (const iBlockHash *, const iBlock *key);
const iAnyObject *  constValue_BlockHash  (const iBlockHash *, const iBlock *key);
iAnyObject *        value_BlockHash       (iBlockHash *, const iBlock *key);

void            clear_BlockHash        (iBlockHash *);

iBool           insert_BlockHash       (iBlockHash *, const iBlock *key, const iAnyObject *value);

/**
 * Insert a key-value element into the BlockHash.
 *
 * @param key    Key string. Ownership taken.
 * @param value  Value object.
 *
 * @return @c iTrue, if the a new key-value element was added and the size of the hash
 * increased as a result. @c False, if an existing one was replaced.
 */
iBool           insertKey_BlockHash    (iBlockHash *, iBlock *key, const iAnyObject *value);

iBool           remove_BlockHash       (iBlockHash *, const iBlock *key);

void            insertValues_BlockHash       (iBlockHash *, const iBlock *key, const iAnyObject *value, ...);
void            insertValuesCStr_BlockHash   (iBlockHash *, const char *key, const iAnyObject *value, ...);

iDeclareIterator(BlockHash, iBlockHash *)
const iBlock *  key_BlockHashIterator(iBlockHashIterator *);
void            remove_BlockHashIterator(iBlockHashIterator *);
struct IteratorImpl_BlockHash {
    iHashIterator base;
    iBlockHashElement *value;
};

iDeclareConstIterator(BlockHash, const iBlockHash *)
const iBlock *  key_BlockHashConstIterator(iBlockHashConstIterator *);
struct ConstIteratorImpl_BlockHash {
    iHashConstIterator base;
    const iBlockHashElement *value;
};

//---------------------------------------------------------------------------------------
// Deriving specialized hashes:

#define iDeclareBlockHash(typeName, keyType, valueType) \
    typedef iBlockHash i##typeName; \
    typedef i##keyType i##typeName##Key; \
    typedef iBlockHashElement i##typeName##Element; \
    typedef iBlockHashElementClass i##typeName##ElementClass; \
    \
    i##typeName##Element *  new_##typeName##Element     (const i##keyType *key, const i##valueType *object); \
    void                    deinit_##typeName##Element  (i##typeName##Element *); \
    const i##keyType *      key_##typeName##Element     (const i##typeName##Element *); \
    void                    initKey_##typeName##Element (const i##typeName##Element *, i##keyType *key_out); \
    i##valueType *          value_##typeName##Element   (const i##typeName##Element *); \
    void                    initBlock_##typeName##Key   (const i##keyType *key, iBlock *); \
    \
    i##typeName *           new_##typeName      (void); \
    void                    delete_##typeName   (i##typeName *); \
    i##typeName *           collect_##typeName  (i##typeName *); \
    \
    void                    init_##typeName     (i##typeName *); \
    void                    deinit_##typeName   (i##typeName *); \
    \
    size_t                  size_##typeName     (const i##typeName *); \
    iBool                   isEmpty_##typeName  (const i##typeName *); \
    \
    iBool                   contains_##typeName     (const i##typeName *, const i##keyType *key); \
    const i##valueType *    constValue_##typeName   (const i##typeName *, const i##keyType *key); \
    i##valueType *          value_##typeName        (i##typeName *, const i##keyType *key); \
    \
    void                    clear_##typeName    (i##typeName *); \
    \
    iBool                   insert_##typeName   (i##typeName *, const i##keyType *key, const i##valueType *value); \
    iBool                   remove_##typeName   (i##typeName *, const i##keyType *key);

/**
 * Things that must be manually defined:
 * - key_<typeName>Element(element): returns the key as-is (if available)
 * - initKey_<typeName>Element(element, key_out): copies the Block back to an existing keyType instance
 * - initBlock_<typeName>Key(key, block): initializes a Block with the key data
 */
#define iDefineBlockHash(typeName, keyType, valueType) \
    i##typeName##Element *new_##typeName##Element(const i##keyType *key, const i##valueType *object) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        void *elem = new_BlockHashElement(&bkey, object); \
        deinit_Block(&bkey); \
        return elem; \
    } \
    void deinit_##typeName##Element(i##typeName##Element *d) { deinit_BlockHashElement(d); } \
    i##valueType *value_##typeName##Element(const i##typeName##Element *d) { \
        return (i##valueType *) d->object; \
    } \
    iDefineTypeConstruction(typeName) \
    void init_##typeName(i##typeName *d) { \
        init_BlockHash(d); \
        setElementClass_BlockHash(d, (const iBlockHashElementClass *) &Class_##typeName##Element); \
    } \
    void deinit_##typeName(i##typeName *d) { \
        deinit_BlockHash(d); \
    } \
    i##typeName *collect_##typeName(i##typeName *d) { return iCollectDel(d, delete_##typeName); } \
    size_t size_##typeName(const i##typeName *d) { return size_BlockHash(d); } \
    iBool isEmpty_##typeName(const i##typeName *d) { return isEmpty_BlockHash(d); } \
    void clear_##typeName(i##typeName *d) { clear_BlockHash(d); } \
    iBool contains_##typeName(const i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = contains_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return res; \
    } \
    const i##valueType *constValue_##typeName(const i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        const i##valueType *obj = (const i##valueType *) constValue_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return obj; \
    } \
    i##valueType *value_##typeName(i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        i##valueType *obj = (i##valueType *) value_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return obj; \
    } \
    iBool insert_##typeName(i##typeName *d, const i##keyType *key, const i##valueType *value) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = insertKey_BlockHash(d, &bkey, value); \
        deinit_Block(&bkey); \
        return res; \
    } \
    iBool remove_##typeName(i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = remove_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return res; \
    }
