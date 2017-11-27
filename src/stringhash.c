/** @file stringhash.c  Hash that uses strings for keys.

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

#include "c_plus/stringhash.h"

#include <stdlib.h>
#include <stdarg.h>

iStringHashElement *new_StringHashElement(iString *key, iAnyObject *object) {
    iStringHashElement *d = iMalloc(StringHashElement);
    d->base.key = key;
    d->object = ref_Object(object);
    return d;
}

void delete_StringHashElement(iStringHashElement *d) {
    if (d) {
        delete_String(d->base.key);
        deref_Object(d->object);
        free(d);
    }
}

static iHashKey keyFunc_StringHash_(const void *key) {
    return crc32_Block(&((const iString *)key)->chars);
}

iStringHash *new_StringHash(void) {
    return new_PtrHash(keyFunc_StringHash_);
}

void delete_StringHash(iStringHash *d) {
    deinit_StringHash(d);
    free(d);
}

void deinit_StringHash(iStringHash *d) {
    clear_StringHash(d); // delete everything
    deinit_PtrHash(d);
}

iBool contains_StringHash(const iStringHash *d, const iString *key) {
    return contains_PtrHash(d, key);
}

const iAnyElement *constValue_StringHash(const iStringHash *d, const iString *key) {
    return constValue_PtrHash(d, key);
}

iAnyElement *value_StringHash(iStringHash *d, const iString *key) {
    return value_PtrHash(d, key);
}

void clear_StringHash(iStringHash *d) {
    iForEach(StringHash, i, d) {
        remove_StringHashIterator(&i);
    }
    clear_PtrHash(d);
}

iBool insert_StringHash(iStringHash *d, const iString *key, iAnyObject *value) {
    return insertKey_StringHash(d, copy_String(key), value);
}

iBool insertKey_StringHash(iStringHash *d, iString *key, iAnyObject *value) {
    iDebug("StringHash: inserting \"%s\" => %s %p\n",
           cstr_String(key),
           class_Object(value)->name, value);
    iStringHashElement *old = insert_PtrHash(d, &new_StringHashElement(key, value)->base);
    if (old) {
        delete_StringHashElement(old);
        return iFalse;
    }
    return iTrue;
}

void insertValues_StringHash(iStringHash *d, const iString *key, iAnyObject *value, ...) {
    insert_StringHash(d, key, value);
    va_list args;
    va_start(args, value);
    for (;;) {
        key = va_arg(args, const iString *);
        if (!key) break;
        value = va_arg(args, iAnyObject *);
        insert_StringHash(d, key, value);
    }
    va_end(args);
}

void insertValuesCStr_StringHash(iStringHash *d, const char *key, iAnyObject *value, ...) {
    insertKey_StringHash(d, newCStr_String(key), value);
    va_list args;
    va_start(args, value);
    for (;;) {
        key = va_arg(args, const char *);
        if (!key) break;
        value = va_arg(args, iAnyObject *);
        insertKey_StringHash(d, newCStr_String(key), value);
    }
    va_end(args);
}

iBool remove_StringHash(iStringHash *d, const iString *key) {
    iStringHashElement *old = remove_PtrHash(d, key);
    if (old) {
        delete_StringHashElement(old);
        return iTrue;
    }
    return iFalse;
}

//---------------------------------------------------------------------------------------

void init_StringHashIterator(iStringHashIterator *d, iStringHash *hash) {
    init_HashIterator(&d->base, &hash->hash);
    d->value = (iStringHashElement *) d->base.value;
}

void next_StringHashIterator(iStringHashIterator *d) {
    next_HashIterator(&d->base);
    d->value = (iStringHashElement *) d->base.value;
}

const iString *key_StringHashIterator(iStringHashIterator *d) {
    return key_StringHashElement(d->value);
}

void remove_StringHashIterator(iStringHashIterator *d) {
    delete_StringHashElement((iStringHashElement *) remove_HashIterator(&d->base));
}

void init_StringHashConstIterator(iStringHashConstIterator *d, const iStringHash *hash) {
    init_HashConstIterator(&d->base, &hash->hash);
    d->value = (const iStringHashElement *) d->base.value;
}

void next_StringHashConstIterator(iStringHashConstIterator *d) {
    next_HashConstIterator(&d->base);
    d->value = (const iStringHashElement *) d->base.value;
}

const iString *key_StringHashConstIterator(iStringHashConstIterator *d) {
    return key_StringHashElement(d->value);
}

