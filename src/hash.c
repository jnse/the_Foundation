/** @file hash.c  Hash of integer values.

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

#include "lite/hash.h"
#include "lite/set.h"

#include <stdlib.h>

typedef iSet iBucket;

iDeclareType(BucketElement);

struct Impl_BucketElement {
    iHashKey key;
    iHashValue value;
};

static int cmp_Bucket_(const void *a, const void *b) {
    return iCmp(((const iBucketElement *) a)->key, ((const iBucketElement *) b)->key);
}

static iBucket *new_Bucket_(void) {
    return new_Set(sizeof(iBucketElement), cmp_Bucket_);
}

static void delete_Bucket_(iBucket *d) {
    delete_Set(d);
}

//---------------------------------------------------------------------------------------

iHash *newBuckets_Hash(size_t buckets) {
    iHash *d = malloc(sizeof(iHash));
    init_Hash(d, buckets);
    return d;
}

void delete_Hash(iHash *d) {
    deinit_Hash(d);
    free(d);
}

void init_Hash(iHash *d, size_t buckets) {
    // Bucket sets are created as needed.
    init_PtrArray(&d->buckets);
    resize_PtrArray(&d->buckets, buckets);
    fill_Array(&d->buckets, 0);
    d->size = 0;
}

void deinit_Hash(iHash *d) {
    // Delete all buckets.
    iForEach(PtrArray, i, &d->buckets) {
        delete_Bucket_(i.value);
    }
    deinit_PtrArray(&d->buckets);
}

iBool       contains_Hash(const iHash *, iHashKey key);

iHashValue  valueDefault_Hash(const iHash *, iHashKey key, iHashValue defaultValue);

void        clear_Hash  (iHash *);

void        insert_Hash (iHash *, iHashKey key, iHashValue value);

iBool       remove_Hash (iHash *, iHashKey key);
