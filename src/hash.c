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

static int cmp_Bucket_(const void *a, const void *b) {
    return iCmp(((const iHashElement *) a)->key, ((const iHashElement *) b)->key);
}

static iBucket *new_Bucket_(void) {
    return new_Set(sizeof(iHashElement), cmp_Bucket_);
}

static void delete_Bucket_(iBucket *d) {
    delete_Set(d);
}

static iHashElement *element_Bucket_(const iBucket *d, size_t pos) {
    return at_Set(d, pos);
}

//---------------------------------------------------------------------------------------

static size_t bucketIndex_Hash_(const iHash *d, iHashKey key) {
    return key % size_Array(&d->buckets);
}

static const iBucket *constFindBucket_Hash_(const iHash *d, iHashKey key) {
    return at_PtrArray(&d->buckets, bucketIndex_Hash_(d, key));
}

static iBucket *findBucket_Hash_(iHash *d, iHashKey key) {
    iBucket **bucket = at_Array(&d->buckets, bucketIndex_Hash_(d, key));
    if (!*bucket) {
        *bucket = new_Bucket_();
    }
    return *bucket;
}

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
    d->size = 0;
}

void deinit_Hash(iHash *d) {
    // Delete all buckets.
    iForEach(PtrArray, i, &d->buckets) {
        delete_Bucket_(i.ptr);
    }
    deinit_PtrArray(&d->buckets);
}

iBool contains_Hash(const iHash *d, iHashKey key) {
    if (isEmpty_Hash(d)) return iFalse;
    const iBucket *bucket = constFindBucket_Hash_(d, key);
    return bucket && contains_Set(bucket, &key);
}

iHashValue valueDefault_Hash(const iHash *d, iHashKey key, iHashValue defaultValue) {
    if (isEmpty_Hash(d)) return defaultValue;
    const iBucket *bucket = constFindBucket_Hash_(d, key);
    iRanges found;
    if (!locate_Set(bucket, &key, &found)) {
        return defaultValue;
    }
    return element_Bucket_(bucket, found.start)->value;
}

void clear_Hash(iHash *d) {
    iForEach(PtrArray, i, &d->buckets) {
        delete_Bucket_(i.ptr);
    }
    fill_Array(&d->buckets, 0);
    d->size = 0;
}

void insert_Hash(iHash *d, iHashKey key, iHashValue value) {
    const iHashElement elem = { key, value };
    if (insert_Set(findBucket_Hash_(d, key), &elem)) {
        d->size++;
    }
}

iBool remove_Hash(iHash *d, iHashKey key) {
    if (isEmpty_Hash(d)) return iFalse;
    iBucket *bucket = findBucket_Hash_(d, key);
    if (bucket && remove_Set(bucket, &key)) {
        d->size--;
    }
    return iFalse;
}

//---------------------------------------------------------------------------------------

#define bucket_Hash_(d, pos)                ((iBucket *) at_PtrArray(&(d)->buckets, pos))
#define bucketElement_Hash_(d, buck, pos)   element_Bucket_(bucket_Hash_(d, buck), pos)

static int findNextBucket_Hash_(const iHash *d, int buck) {
    for (buck++; buck < size_PtrArray(&d->buckets); ++buck) {
        const iBucket *bucket = bucket_Hash_(d, buck);
        if (bucket && !isEmpty_Set(bucket)) return buck;
    }
    return -1;
}

void init_HashIterator(iHashIterator *d, iHash *hash) {
    d->hash = hash;
    if (hash->size > 0) {
        d->bucket = findNextBucket_Hash_(hash, -1);
        d->pos = 0;
        iHashElement *elem = bucketElement_Hash_(hash, d->bucket, d->pos);
        d->value = &elem->value;
        d->key = elem->key;
    }
    else {
        d->value = NULL;
    }
}

void next_HashIterator(iHashIterator *d) {
    if (++d->pos == size_Set(bucket_Hash_(d->hash, d->bucket))) {
        d->pos = 0;
        d->bucket = findNextBucket_Hash_(d->hash, d->bucket);
    }
    if (d->bucket >= 0) {
        iHashElement *elem = bucketElement_Hash_(d->hash, d->bucket, d->pos);
        d->value = &elem->value;
        d->key = elem->key;
    }
    else {
        d->value = NULL;
    }
}
