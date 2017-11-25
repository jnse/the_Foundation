#pragma once

/** @file lite/hash.h  Hash of integer values.

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

#include "defs.h"
#include "lite/ptrarray.h"
#include "lite/list.h"

iDeclareType(Hash);
iDeclareType(HashElement);

#define iHashDefaultBuckets 53

typedef uint32_t iHashKey;
typedef intptr_t iHashValue;

struct Impl_Hash {
    iPtrArray buckets;
    size_t size;
};

struct Impl_HashElement {
    iHashKey key;
    iHashValue value;
};

#define     new_Hash()      newBuckets_Hash(iHashDefaultBuckets)

iHash *     newBuckets_Hash (size_t buckets);
void        delete_Hash     (iHash *);

#define     collect_Hash(d) iCollectDel(d, delete_iHash)

void        init_Hash   (iHash *, size_t buckets);
void        deinit_Hash (iHash *);

#define     size_Hash(d)    ((d)->size)
#define     isEmpty_Hash(d) (size_Hash(d) == 0)

iBool       contains_Hash(const iHash *, iHashKey key);

#define     value_Hash(d, key)  valueDefault_Hash(d, key, 0)

iHashValue  valueDefault_Hash(const iHash *, iHashKey key, iHashValue defaultValue);

void        clear_Hash  (iHash *);
void        insert_Hash (iHash *, iHashKey key, iHashValue value);
iBool       remove_Hash (iHash *, iHashKey key);

iDeclareIterator(Hash, iHash *);
iDeclareConstIterator(Hash, const iHash *);

struct IteratorImpl_Hash {
    iHashValue *value;
    iHashKey key;
    int bucket;
    size_t pos;
    iHash *hash;
};
struct ConstIteratorImpl_Hash {
    const iHashValue *value;
    iHashKey key;
    int bucket;
    size_t pos;
    const iHash *hash;
};
