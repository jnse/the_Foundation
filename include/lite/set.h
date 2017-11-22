#pragma once

/** @file lite/set.h  Set of unique integer values.

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

#include "array.h"

struct i_Set_Impl {
    iArray values;
};

LITE_DECLARE_IMPL(Set);

typedef intptr_t iSetValue;

iSet *      iSet_new(void);
void        iSet_delete(iSet *);

void        iSet_init(iSet *d);
void        iSet_deinit(iSet *d);

#define     iSet_isEmpty(d) iArray_isEmpty(&(d)->values)

size_t      iSet_size(const iSet *);
iBool       iSet_contains(const iSet *, iSetValue value);
iBool       iSet_locate(const iSet *, iSetValue value, iRanges *outLoc);
iSetValue   iSet_at(const iSet *, size_t pos);

void        iSet_clear(iSet *);
iBool       iSet_insert(iSet *, iSetValue value);
iBool       iSet_remove(iSet *, iSetValue value);
