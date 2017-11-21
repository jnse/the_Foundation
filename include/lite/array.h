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

struct i_Array_Impl {
    char *data;
    iRanges range;
    size_t elementSize;
    size_t allocSize;
};

LITE_DECLARE_IMPL(Array);

iArray *    iArray_new      (size_t elementSize);
void        iArray_delete   (iArray *);

#define     iArray_isEmpty(d)   iRange_isEmpty(&(d)->range)
#define     iArray_size(d)      iRange_size(&(d)->range)
#define     iArray_front(d)     iArray_at(d, 0)
#define     iArray_back(d)      iArray_at(d, iArray_size(d) - 1)

void *      iArray_data (const iArray *);
void *      iArray_at   (const iArray *, size_t pos);

void        iArray_reserve      (iArray *, size_t reservedSize);
void        iArray_resize       (iArray *, size_t size);
void        iArray_pushBack     (iArray *, const void *value);
void        iArray_pushFront    (iArray *, const void *value);
iBool       iArray_popBack      (iArray *);
iBool       iArray_popFront     (iArray *);
iBool       iArray_take         (iArray *, size_t pos, void *outValue);
void        iArray_insert       (iArray *, size_t pos, const void *value);
void        iArray_remove       (iArray *, size_t pos);
