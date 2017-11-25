#pragma once

/** @file lite/array.h  Array of sequential fixed-size elements.

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
#include "range.h"

struct Impl_Array {
    char *data;
    iRanges range;
    size_t elementSize;
    size_t allocSize;
};

iDeclareType(Array);

iArray *    new_Array      (size_t elementSize);
void        delete_Array   (iArray *);

void        init_Array     (iArray *, size_t elementSize);
void        deinit_Array   (iArray *);

#define     isEmpty_Array(d)    isEmpty_Range(&(d)->range)
#define     size_Array(d)       size_Range(&(d)->range)
#define     front_Array(d)      at_Array(d, 0)
#define     back_Array(d)       at_Array(d, size_Array(d) - 1)

void *      data_Array      (const iArray *);
void *      at_Array        (const iArray *, size_t pos);
const void *constEnd_Array  (const iArray *);

void        reserve_Array   (iArray *, size_t reservedSize);
void        clear_Array     (iArray *);
void        resize_Array    (iArray *, size_t size);
void        set_Array       (iArray *, size_t pos, const void *value);
void        pushBack_Array  (iArray *, const void *value);
void        pushFront_Array (iArray *, const void *value);
iBool       popBack_Array   (iArray *);
iBool       popFront_Array  (iArray *);
iBool       take_Array      (iArray *, size_t pos, void *outValue);
void        insert_Array    (iArray *, size_t pos, const void *value);
void        remove_Array    (iArray *, size_t pos);
void        fill_Array      (iArray *, char value);
void        sort_Array      (iArray *, int (*cmp)(const void *, const void *));

iDeclareIterator(Array, iArray *);
iDeclareConstIterator(Array, const iArray *);

struct IteratorImpl_Array {
    void *value; // address of element
    iArray *array;
    size_t pos;
};
struct ConstIteratorImpl_Array {
    const void *value; // address of element
    const iArray *array;
    const void *end;
};
