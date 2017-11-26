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

static void deleteKeys_StringHash_(iStringHash *d) {
    iConstForEach(Hash, i, &d->hash) {
        delete_String(((const iStringHashElement *) i.value)->key);
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
    // Key strings are owned by the hash.
    deleteKeys_StringHash_(d);
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
    deleteKeys_StringHash_(d);
    clear_PtrHash(d);
}

iAnyElement *insert_StringHash(iStringHash *d, const iString *key, iAnyElement *element) {
    return insertKey_StringHash(d, copy_String(key), element);
}

iAnyElement *insertKey_StringHash(iStringHash *d, iString *key, iAnyElement *element) {
    ((iStringHashElement *)element)->key = key; // ownership taken
    iStringHashElement *old = insert_PtrHash(d, element);
    if (old) {
        delete_String(old->key);
        old->key = NULL;
    }
    return old;
}

void insertElements_StringHash(iStringHash *d, const iString *key, iAnyElement *element, ...) {
    insert_StringHash(d, key, element);
    va_list args;
    va_start(args, element);
    for (;;) {
        key = va_arg(args, const iString *);
        if (!key) break;
        element = va_arg(args, iAnyElement *);
        insert_StringHash(d, key, element);
    }
    va_end(args);
}

void insertElementsCStr_StringHash(iStringHash *d, const char *key, iAnyElement *element, ...) {
    insertKey_StringHash(d, fromCStr_String(key), element);
    va_list args;
    va_start(args, element);
    for (;;) {
        key = va_arg(args, const char *);
        if (!key) break;
        element = va_arg(args, iAnyElement *);
        insertKey_StringHash(d, fromCStr_String(key), element);
    }
    va_end(args);
}

iAnyElement *remove_StringHash(iStringHash *d, const iString *key) {
    iStringHashElement *old = remove_PtrHash(d, key);
    if (old) {
        delete_String(old->key);
        old->key = NULL;
    }
    return old;
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
    return ((const iStringHashElement *)d->base.value)->key;
}

iAnyElement *remove_StringHashIterator(iStringHashIterator *d) {
    iStringHashElement *elem = (iStringHashElement *) remove_HashIterator(&d->base);
    delete_String(elem->key);
    elem->key = NULL;
    return elem;
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
    return ((const iStringHashElement *)d->base.value)->key;
}
