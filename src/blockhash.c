/** @file blockhash.c  Hash that uses Block for keys and Object for values.

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

#include "c_plus/blockhash.h"

#include <stdlib.h>
#include <stdarg.h>

iBlockHashElement *new_BlockHashElement(const iBlock *key, iAnyObject *object) {
    iBlockHashElement *d = iMalloc(BlockHashElement);
    initCopy_Block(&d->keyBlock, key);
    d->object = ref_Object(object);
    return d;
}

void delete_BlockHashElement(iBlockHashElement *d) {
    if (d) {
        deinit_Block(key_BlockHashElement(d));
        deref_Object(d->object);
        free(d);
    }
}

static iHashKey keyFunc_BlockHash_(const void *key) {
    return crc32_Block((const iBlock *) key);
}

iBlockHash *new_BlockHash(void) {
    return new_PtrHash(keyFunc_BlockHash_);
}

void delete_BlockHash(iBlockHash *d) {
    deinit_BlockHash(d);
    free(d);
}

void deinit_BlockHash(iBlockHash *d) {
    clear_BlockHash(d); // delete everything
    deinit_PtrHash(d);
}

iBool contains_BlockHash(const iBlockHash *d, const iBlock *key) {
    return contains_PtrHash(d, key);
}

const iAnyElement *constValue_BlockHash(const iBlockHash *d, const iBlock *key) {
    return constValue_PtrHash(d, key);
}

iAnyElement *value_BlockHash(iBlockHash *d, const iBlock *key) {
    return value_PtrHash(d, key);
}

void clear_BlockHash(iBlockHash *d) {
    iForEach(BlockHash, i, d) {
        remove_BlockHashIterator(&i);
    }
    clear_PtrHash(d);
}

iBool insert_BlockHash(iBlockHash *d, const iBlock *key, iAnyObject *value) {
    return insertKey_BlockHash(d, copy_Block(key), value);
}

iBool insertKey_BlockHash(iBlockHash *d, iBlock *key, iAnyObject *value) {
    iDebug("BlockHash: inserting \"%s\" => %s %p\n",
           constData_Block(key),
           class_Object(value)->name, value);
    iBlockHashElement *old = insert_PtrHash(d, &new_BlockHashElement(key, value)->base);
    if (old) {
        delete_BlockHashElement(old);
        return iFalse;
    }
    return iTrue;
}

void insertValues_BlockHash(iBlockHash *d, const iBlock *key, iAnyObject *value, ...) {
    insert_BlockHash(d, key, value);
    va_list args;
    va_start(args, value);
    for (;;) {
        key = va_arg(args, const iBlock *);
        if (!key) break;
        value = va_arg(args, iAnyObject *);
        insert_BlockHash(d, key, value);
    }
    va_end(args);
}

void insertValuesCStr_BlockHash(iBlockHash *d, const char *key, iAnyObject *value, ...) {
    insertKey_BlockHash(d, newCStr_Block(key), value);
    va_list args;
    va_start(args, value);
    for (;;) {
        key = va_arg(args, const char *);
        if (!key) break;
        value = va_arg(args, iAnyObject *);
        insertKey_BlockHash(d, newCStr_Block(key), value);
    }
    va_end(args);
}

iBool remove_BlockHash(iBlockHash *d, const iBlock *key) {
    iBlockHashElement *old = remove_PtrHash(d, key);
    if (old) {
        delete_BlockHashElement(old);
        return iTrue;
    }
    return iFalse;
}

//---------------------------------------------------------------------------------------

void init_BlockHashIterator(iBlockHashIterator *d, iBlockHash *hash) {
    init_HashIterator(&d->base, &hash->hash);
    d->value = (iBlockHashElement *) d->base.value;
}

void next_BlockHashIterator(iBlockHashIterator *d) {
    next_HashIterator(&d->base);
    d->value = (iBlockHashElement *) d->base.value;
}

const iBlock *key_BlockHashIterator(iBlockHashIterator *d) {
    return key_BlockHashElement(d->value);
}

void remove_BlockHashIterator(iBlockHashIterator *d) {
    delete_BlockHashElement((iBlockHashElement *) remove_HashIterator(&d->base));
}

void init_BlockHashConstIterator(iBlockHashConstIterator *d, const iBlockHash *hash) {
    init_HashConstIterator(&d->base, &hash->hash);
    d->value = (const iBlockHashElement *) d->base.value;
}

void next_BlockHashConstIterator(iBlockHashConstIterator *d) {
    next_HashConstIterator(&d->base);
    d->value = (const iBlockHashElement *) d->base.value;
}

const iBlock *key_BlockHashConstIterator(iBlockHashConstIterator *d) {
    return key_BlockHashElement(d->value);
}

