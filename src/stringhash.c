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
#include "c_plus/garbage.h"

#include <stdlib.h>
#include <stdarg.h>

static iBeginDefineClass(StringHashElement)
    .new = (iBlockHashElement *(*)(const iBlock *, const iAnyObject *)) new_StringHashElement,
    .hashKey = hashKey_BlockHashElement,
iEndDefineClass(StringHashElement)

iDefineBlockHash(StringHash, String, AnyObject)

const iString *key_StringHashElement(const iStringHashElement *d) {
    return (const iString *) &d->keyBlock; // iString derived from iBlock
}

void initKey_StringHashElement(const iStringHashElement *d, iString *key_out) {
    init_String(key_out, &d->keyBlock);
}

void initBlock_StringHashKey(const iString *d, iBlock *block) {
    initCopy_Block(block, &d->chars);
}

void insertValues_StringHash(iStringHash *d, const iString *key, iAnyObject *value, ...) {
    insert_StringHash(d, copy_String(key), value);
    va_list args;
    for (va_start(args, value);;) {
        key = va_arg(args, const iString *);
        if (!key) break;
        value = va_arg(args, iAnyObject *);
        insert_StringHash(d, copy_String(key), value);
    }
    va_end(args);
}

void insertValuesCStr_StringHash(iStringHash *d, const char *key, iAnyObject *value, ...) {
    insert_StringHash(d, newCStr_String(key), value);
    va_list args;
    for (va_start(args, value);;) {
        key = va_arg(args, const char *);
        if (!key) break;
        value = va_arg(args, iAnyObject *);
        insert_StringHash(d, newCStr_String(key), value);
    }
    va_end(args);
}

//---------------------------------------------------------------------------------------

void init_StringHashIterator(iStringHashIterator *d, iStringHash *hash) {
    init_HashIterator(&d->base, &hash->base);
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
    remove_BlockHashIterator((iBlockHashIterator *) d);
}

void init_StringHashConstIterator(iStringHashConstIterator *d, const iStringHash *hash) {
    init_HashConstIterator(&d->base, &hash->base);
    d->value = (const iStringHashElement *) d->base.value;
}

void next_StringHashConstIterator(iStringHashConstIterator *d) {
    next_HashConstIterator(&d->base);
    d->value = (const iStringHashElement *) d->base.value;
}

const iString *key_StringHashConstIterator(iStringHashConstIterator *d) {
    return key_StringHashElement(d->value);
}
