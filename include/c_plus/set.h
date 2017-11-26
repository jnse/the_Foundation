#pragma once

/** @file c_plus/set.h  Set of unique values.

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

iDeclareType(Set);

typedef int (*iSetCmpElem)(const void *, const void *);

struct Impl_Set {
    iArray values;
    iSetCmpElem cmp;
};

iSet *      new_Set     (size_t elementSize, iSetCmpElem cmp);
void        delete_Set  (iSet *);

void        init_Set    (iSet *d, size_t elementSize, iSetCmpElem cmp);
void        deinit_Set  (iSet *d);

size_t      size_Set    (const iSet *);
iBool       contains_Set(const iSet *, const void *value);
iBool       locate_Set  (const iSet *, const void *value, iRanges *outLoc);

#define     at_Set(d, pos)  at_Array(&(d)->values, pos)
#define     isEmpty_Set(d)  isEmpty_Array(&(d)->values)

void        clear_Set   (iSet *);
iBool       insert_Set  (iSet *, const void *value);
iBool       remove_Set  (iSet *, const void *value);

iDeclareIterator(Set, iSet *);
iDeclareConstIterator(Set, const iSet *);
