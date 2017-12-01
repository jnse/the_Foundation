#pragma once

/** @file c_plus/blockhash.h  Hash object that uses Block for keys and Object for values.

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

#include "hash.h"
#include "block.h"
#include "object.h"

iDeclareType(BlockHash)
iDeclareType(BlockHashNode)

iDeclareClass(BlockHash)

iBeginDeclareClass(BlockHashNode)
    iBlockHashNode *    (*new)      (const iBlock *key, const iAnyObject *object);
    iHashKey            (*hashKey)  (const iBlock *key);
iEndDeclareClass(BlockHashNode)

struct Impl_BlockHash {
    iObject object;
    iHash hash;
    const iBlockHashNodeClass *nodeClass;
};

struct Impl_BlockHashNode {
    iHashNode base;
    iBlock keyBlock;
    iAnyObject *object;
};

iBlockHashNode *    new_BlockHashNode    (const iBlock *key, const iAnyObject *object);
iHashKey            hashKey_BlockHashNode(const iBlock *key);
void                deinit_BlockHashNode (iBlockHashNode *);

#define         key_BlockHashNode(d)    iConstCast(iBlock *, (&((const iBlockHashNode *) (d))->keyBlock))

iBlockHash *    new_BlockHash     (void);
void            delete_BlockHash  (iBlockHash *);

#define         collect_BlockHash(d)    iCollectDel(d, delete_BlockHash)

void            init_BlockHash    (iBlockHash *);
void            deinit_BlockHash  (iBlockHash *);

void            setNodeClass_BlockHash  (iBlockHash *, const iBlockHashNodeClass *class);

#define         size_BlockHash(d)       size_Hash(&(d)->hash)
#define         isEmpty_BlockHash(d)    isEmpty_Hash(&(d)->hash)

iBool               contains_BlockHash    (const iBlockHash *, const iBlock *key);
const iAnyObject *  constValue_BlockHash  (const iBlockHash *, const iBlock *key);
iAnyObject *        value_BlockHash       (iBlockHash *, const iBlock *key);

void            clear_BlockHash        (iBlockHash *);

/**
 * Insert a key-value node into the BlockHash.
 *
 * @param key    Key string. A copy is made of the key and stored in the node.
 * @param value  Value object.
 *
 * @return @c iTrue, if the a new key-value node was added and the size of the hash
 * increased as a result. @c False, if an existing one was replaced.
 */
iBool           insert_BlockHash       (iBlockHash *, const iBlock *key, const iAnyObject *value);

iBool           remove_BlockHash       (iBlockHash *, const iBlock *key);

void            insertValues_BlockHash       (iBlockHash *, const iBlock *key, const iAnyObject *value, ...);
void            insertValuesCStr_BlockHash   (iBlockHash *, const char *key, const iAnyObject *value, ...);

iDeclareIterator(BlockHash, iBlockHash *)
const iBlock *  key_BlockHashIterator(iBlockHashIterator *);
void            remove_BlockHashIterator(iBlockHashIterator *);
struct IteratorImpl_BlockHash {
    iHashIterator base;
    iBlockHashNode *value;
    iBlockHash *blockHash;
};

iDeclareConstIterator(BlockHash, const iBlockHash *)
const iBlock *  key_BlockHashConstIterator(iBlockHashConstIterator *);
struct ConstIteratorImpl_BlockHash {
    iHashConstIterator base;
    const iBlockHashNode *value;
};

//---------------------------------------------------------------------------------------
// Deriving specialized hashes:

#define iDeclareBlockHash(typeName, keyType, valueType) \
    typedef iBlockHash i##typeName; \
    typedef i##keyType i##typeName##Key; \
    typedef iBlockHashNode i##typeName##Node; \
    typedef iBlockHashNodeClass i##typeName##NodeClass; \
    iDeclareClass(typeName) \
    \
    i##typeName##Node *     new_##typeName##Node        (const i##keyType *key, const i##valueType *object); \
    void                    deinit_##typeName##Node     (i##typeName##Node *); \
    const i##keyType *      key_##typeName##Node        (const i##typeName##Node *); \
    void                    initKey_##typeName##Node    (const i##typeName##Node *, i##keyType *key); \
    i##valueType *          value_##typeName##Node      (const i##typeName##Node *); \
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
    iBool                   remove_##typeName   (i##typeName *, const i##keyType *key); \
    \
    iDeclareIterator(typeName, i##typeName *) \
    const i##keyType *      key_##typeName##Iterator(i##typeName##Iterator *); \
    void                    remove_##typeName##Iterator(i##typeName##Iterator *); \
    struct IteratorImpl_##typeName { \
        iHashIterator base; \
        i##typeName##Node *value; \
    }; \
    \
    iDeclareConstIterator(typeName, const i##typeName *) \
    const i##keyType * key_##typeName##ConstIterator(i##typeName##ConstIterator *); \
    struct ConstIteratorImpl_##typeName { \
        iHashConstIterator base; \
        const i##typeName##Node *value; \
    };

/**
 * Things that must be manually defined:
 * - key_<typeName>Node(d): returns the key as-is (if available)
 * - initKey_<typeName>Node(d, key): copies the Block back to an existing keyType instance
 * - initBlock_<typeName>Key(key, block): initializes a Block with the key data
 */
#define iDefineBlockHash(typeName, keyType, valueType) \
    iDefineClass(typeName) \
    iDefineObjectConstruction(typeName) \
    \
    i##typeName##Node *new_##typeName##Node(const i##keyType *key, const i##valueType *object) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        void *elem = new_BlockHashNode(&bkey, object); \
        deinit_Block(&bkey); \
        return elem; \
    } \
    \
    void deinit_##typeName##Node(i##typeName##Node *d) { deinit_BlockHashNode(d); } \
    i##valueType *value_##typeName##Node(const i##typeName##Node *d) { \
        return (i##valueType *) d->object; \
    } \
    \
    void init_##typeName(i##typeName *d) { \
        init_BlockHash(d); \
        setNodeClass_BlockHash(d, (const iBlockHashNodeClass *) &Class_##typeName##Node); \
    } \
    \
    void deinit_##typeName(i##typeName *d) { \
        deinit_BlockHash(d); \
    } \
    \
    i##typeName *collect_##typeName(i##typeName *d) { return iCollectDel(d, delete_##typeName); } \
    \
    size_t size_##typeName(const i##typeName *d) { return size_BlockHash(d); } \
    \
    iBool isEmpty_##typeName(const i##typeName *d) { return isEmpty_BlockHash(d); } \
    \
    void clear_##typeName(i##typeName *d) { clear_BlockHash(d); } \
    \
    iBool contains_##typeName(const i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = contains_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return res; \
    } \
    \
    const i##valueType *constValue_##typeName(const i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        const i##valueType *obj = (const i##valueType *) constValue_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return obj; \
    } \
    \
    i##valueType *value_##typeName(i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        i##valueType *obj = (i##valueType *) value_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return obj; \
    } \
    \
    iBool insert_##typeName(i##typeName *d, const i##keyType *key, const i##valueType *value) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = insert_BlockHash(d, &bkey, value); \
        deinit_Block(&bkey); \
        return res; \
    } \
    \
    iBool remove_##typeName(i##typeName *d, const i##keyType *key) { \
        iBlock bkey; initBlock_##typeName##Key(key, &bkey); \
        iBool res = remove_BlockHash(d, &bkey); \
        deinit_Block(&bkey); \
        return res; \
    } \
    \
    void init_##typeName##Iterator(i##typeName##Iterator *d, i##typeName *hash) { \
        init_HashIterator(&d->base, &hash->hash); \
        d->value = (i##typeName##Node *) d->base.value; \
    } \
    \
    void next_##typeName##Iterator(i##typeName##Iterator *d) { \
        next_HashIterator(&d->base); \
        d->value = (i##typeName##Node *) d->base.value; \
    } \
    \
    const i##keyType *key_##typeName##Iterator(i##typeName##Iterator *d) { \
        return key_##typeName##Node(d->value); \
    } \
    \
    void remove_##typeName##Iterator(i##typeName##Iterator *d) { \
        remove_BlockHashIterator((iBlockHashIterator *) d); \
    } \
    \
    void init_##typeName##ConstIterator(i##typeName##ConstIterator *d, const i##typeName *hash) { \
        init_HashConstIterator(&d->base, &hash->hash); \
        d->value = (const i##typeName##Node *) d->base.value; \
    } \
    \
    void next_##typeName##ConstIterator(i##typeName##ConstIterator *d) { \
        next_HashConstIterator(&d->base); \
        d->value = (const i##typeName##Node *) d->base.value; \
    } \
    \
    const i##keyType *key_##typeName##ConstIterator(i##typeName##ConstIterator *d) { \
        return key_##typeName##Node(d->value); \
    }
