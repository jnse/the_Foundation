#pragma once

/** @file c_plus/stringhash.h  Hash that uses String for keys and Object for values.

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

#include "blockhash.h"
#include "string.h"

iDeclareBlockHash(StringHash, String, AnyObject)

#if 0
iDeclareType(StringHashElement)

typedef iPtrHash iStringHash;

struct Impl_StringHashElement {
    iPtrHashElement base; // key String held here
    iAnyObject *object;
};

iStringHashElement *    new_StringHashElement(iString *key, iAnyObject *object);
void                    delete_StringHashElement(iStringHashElement *);

#define         key_StringHashElement(d)    (iConstCast(iString *, (d)->base.key))

iStringHash *   new_StringHash     (void);
void            delete_StringHash  (iStringHash *);

#define         collect_StringHash(d)       iCollectDel(d, delete_StringHash)

void            init_StringHash    (iStringHash *);
void            deinit_StringHash  (iStringHash *);

#define         size_StringHash(d)          size_Hash(&(d)->hash)
#define         isEmpty_StringHash(d)       isEmpty_Hash(&(d)->hash)

iBool               contains_StringHash    (const iStringHash *, const iString *key);
const iAnyObject *  constValue_StringHash  (const iStringHash *, const iString *key);
iAnyObject *        value_StringHash       (iStringHash *, const iString *key);

void            clear_StringHash        (iStringHash *);

iBool           insert_StringHash       (iStringHash *, const iString *key, iAnyObject *value);

/**
 * Insert a key-value element into the StringHash.
 *
 * @param key    Key string. Ownership taken.
 * @param value  Value object.
 *
 * @return @c iTrue, if the a new key-value element was added and the size of the hash
 * increased as a result. @c False, if an existing one was replaced.
 */
iBool           insertKey_StringHash    (iStringHash *, iString *key, iAnyObject *value);

iBool           remove_StringHash       (iStringHash *, const iString *key);

#endif

void            insertValues_StringHash       (iStringHash *, const iString *key, iAnyObject *value, ...);
void            insertValuesCStr_StringHash   (iStringHash *, const char *key, iAnyObject *value, ...);

iDeclareIterator(StringHash, iStringHash *)
const iString * key_StringHashIterator(iStringHashIterator *);
void            remove_StringHashIterator(iStringHashIterator *);
struct IteratorImpl_StringHash {
    iHashIterator base;
    iStringHashElement *value;
};

iDeclareConstIterator(StringHash, const iStringHash *)
const iString *key_StringHashConstIterator(iStringHashConstIterator *);
struct ConstIteratorImpl_StringHash {
    iHashConstIterator base;
    const iStringHashElement *value;
};
