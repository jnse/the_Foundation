#pragma once

/** @file c_plus/stringhash.h  Hash that uses strings for keys.

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

iDeclareType(StringHashElement);

typedef iPtrHash iStringHash;

struct Impl_StringHashElement { // compatible with iPtrHashElement
    iHashElement base;
    iString *key;
};

iStringHash *   new_StringHash              (void);
void            delete_StringHash           (iStringHash *);

#define         collect_StringHash(d) iCollectDel(d, delete_StringHash)

void            init_StringHash    (iStringHash *);
void            deinit_StringHash  (iStringHash *);

#define         size_StringHash(d)         size_Hash(&(d)->hash)
#define         isEmpty_StringHash(d)      isEmpty_Hash(&(d)->hash)

iBool               contains_StringHash    (const iStringHash *, const iString *key);
const iAnyElement * constValue_StringHash  (const iStringHash *, const iString *key);
iAnyElement *       value_StringHash       (iStringHash *, const iString *key);

void            clear_StringHash        (iStringHash *);

iAnyElement *   insert_StringHash       (iStringHash *, const iString *key, iAnyElement *element);
iAnyElement *   insertKey_StringHash    (iStringHash *, iString *key, iAnyElement *element);
iAnyElement *   remove_StringHash       (iStringHash *, const iString *key);

void            insertElments_StringHash        (iStringHash *, const iString *key, iAnyElement *element, ...);
void            insertElementsCStr_StringHash   (iStringHash *, const char *key, iAnyElement *element, ...);

iDeclareIterator(StringHash, iStringHash *);
const iString * key_StringHashIterator(iStringHashIterator *);
iAnyElement *   remove_StringHashIterator(iStringHashIterator *);
struct IteratorImpl_StringHash {
    iHashIterator base;
    iStringHashElement *value;
};

iDeclareConstIterator(StringHash, const iStringHash *);
const iString *key_StringHashConstIterator(iStringHashConstIterator *);
struct ConstIteratorImpl_StringHash {
    iHashConstIterator base;
    const iStringHashElement *value;
};
