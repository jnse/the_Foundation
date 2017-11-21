#pragma once

/** @file lite/ptrset.h  Set of unique pointers.

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

#include "set.h"

typedef iSet iPtrSet;

iPtrSet *   iPtrSet_new(void);
void        iPtrSet_delete(iPtrSet *);

#define iPtrSet_init(d)     iSet_init(d)
#define iPtrSet_deinit(d)   iSet_deinit(d)

#define iPtrSet_isEmpty(d)  iSet_isEmpty(d)
#define iPtrSet_size(d)     iSet_size(d)

iBool       iPtrSet_contains(const iPtrSet *, void *ptr);
iBool       iPtrSet_locate(const iPtrSet *, void *ptr, iRanges *outLoc);
void *      iPtrSet_at(const iSet *, size_t pos);

iBool       iPtrSet_insert(iPtrSet *, void *ptr);
iBool       iPtrSet_remove(iPtrSet *, void *ptr);
