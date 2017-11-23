/** @file ptrset.c  Set of unique pointers.

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

#include "lite/ptrset.h"

iPtrSet *new_PtrSet(void) {
    return new_Set();
}

void delete_PtrSet(iPtrSet *d) {
    delete_Set(d);
}

iBool contains_PtrSet(const iPtrSet *d, void *ptr) {
    return contains_Set(d, (iSetValue) ptr);
}

iBool locate_PtrSet(const iPtrSet *d, void *ptr, iRanges *outLoc) {
    return locate_Set(d, (iSetValue) ptr, outLoc);
}

iBool insert_PtrSet(iPtrSet *d, void *ptr) {
    return insert_Set(d, (iSetValue) ptr);
}

iBool remove_PtrSet(iPtrSet *d, void *ptr) {
    return remove_Set(d, (iSetValue) ptr);
}

void *at_PtrSet(const iSet *d, size_t pos) {
    return (void *) at_Set(d, pos);
}