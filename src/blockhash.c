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
#include "c_plus/garbage.h"

#include <stdlib.h>
#include <stdarg.h>

iBlockHashElement *new_BlockHashElement(const iBlock *key, const iAnyObject *object) {
    iBlockHashElement *d = iMalloc(BlockHashElement);
    initCopy_Block(&d->keyBlock, key);
    d->object = ref_Object(object);
    return d;
}

void deinit_BlockHashElement(iBlockHashElement *d) {
    if (d) {
        deinit_Block(key_BlockHashElement(d));
        deref_Object(d->object);
    }
}

iHashKey hashKey_BlockHashElement(const iBlock *key) {
    return crc32_Block(key);
}

iBeginDefineClass(BlockHashElement)
    .new     = new_BlockHashElement,
    .hashKey = hashKey_BlockHashElement,
iEndDefineClass(BlockHashElement)

//---------------------------------------------------------------------------------------

iDefineTypeConstruction(BlockHash)

void init_BlockHash(iBlockHash *d) {
    init_Hash(&d->base);
    setElementClass_BlockHash(d, &Class_BlockHashElement);
}

void deinit_BlockHash(iBlockHash *d) {
    clear_BlockHash(d); // delete everything
    deinit_Hash(&d->base);
}

void setElementClass_BlockHash(iBlockHash *d, const iBlockHashElementClass *class) {
    d->elementClass = class;
}

iBool contains_BlockHash(const iBlockHash *d, const iBlock *key) {
    return contains_Hash(&d->base, d->elementClass->hashKey(key));
}

const iAnyElement *constValue_BlockHash(const iBlockHash *d, const iBlock *key) {
    return value_Hash(&d->base, d->elementClass->hashKey(key));
}

iAnyElement *value_BlockHash(iBlockHash *d, const iBlock *key) {
    return value_Hash(&d->base, d->elementClass->hashKey(key));
}

void clear_BlockHash(iBlockHash *d) {
    iForEach(BlockHash, i, d) {
        remove_BlockHashIterator(&i);
    }
    clear_Hash(&d->base);
}

iBool insert_BlockHash(iBlockHash *d, const iBlock *key, const iAnyObject *value) {
    iDebug("BlockHash: inserting \"%s\" => %s %p\n",
           constData_Block(key),
           class_Object(value)->name, value);
    iAnyElement *old = insert_Hash(&d->base, (iHashElement *) d->elementClass->new(key, value));
    if (old) {
        delete_Class(d->elementClass, old);
        return iFalse;
    }
    return iTrue;
}

void insertValues_BlockHash(iBlockHash *d, const iBlock *key, const iAnyObject *value, ...) {
    insert_BlockHash(d, key, value);
    va_list args;
    for (va_start(args, value);;) {
        key = va_arg(args, const iBlock *);
        if (!key) break;
        insert_BlockHash(d, key, va_arg(args, const iAnyObject *));
    }
    va_end(args);
}

void insertValuesCStr_BlockHash(iBlockHash *d, const char *key, const iAnyObject *value, ...) {
    insert_BlockHash(d, iDeref(newCStr_Block(key)), value);
    va_list args;
    for (va_start(args, value);;) {
        key = va_arg(args, const char *);
        if (!key) break;
        insert_BlockHash(d, iDeref(newCStr_Block(key)), va_arg(args, const iAnyObject *));
    }
    va_end(args);
}

iBool remove_BlockHash(iBlockHash *d, const iBlock *key) {
    iHashElement *old = remove_Hash(&d->base, d->elementClass->hashKey(key));
    if (old) {
        delete_Class(d->elementClass, old);
        return iTrue;
    }
    return iFalse;
}

//---------------------------------------------------------------------------------------

void init_BlockHashIterator(iBlockHashIterator *d, iBlockHash *hash) {
    init_HashIterator(&d->base, &hash->base);
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
    delete_Class(((iBlockHash *) d->base.hash)->elementClass, remove_HashIterator(&d->base));
}

void init_BlockHashConstIterator(iBlockHashConstIterator *d, const iBlockHash *hash) {
    init_HashConstIterator(&d->base, &hash->base);
    d->value = (const iBlockHashElement *) d->base.value;
}

void next_BlockHashConstIterator(iBlockHashConstIterator *d) {
    next_HashConstIterator(&d->base);
    d->value = (const iBlockHashElement *) d->base.value;
}

const iBlock *key_BlockHashConstIterator(iBlockHashConstIterator *d) {
    return key_BlockHashElement(d->value);
}
