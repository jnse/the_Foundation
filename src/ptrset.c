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

iPtrSet *iPtrSet_new(void) {
    return iSet_new();
}

void iPtrSet_delete(iPtrSet *d) {
    iSet_delete(d);
}

iBool iPtrSet_contains(const iPtrSet *d, void *ptr) {
    return iSet_contains(d, (iSetValue) ptr);
}

iBool iPtrSet_locate(const iPtrSet *d, void *ptr, iRanges *outLoc) {
    return iSet_locate(d, (iSetValue) ptr, outLoc);
}

iBool iPtrSet_insert(iPtrSet *d, void *ptr) {
    return iSet_insert(d, (iSetValue) ptr);
}

iBool iPtrSet_remove(iPtrSet *d, void *ptr) {
    return iSet_remove(d, (iSetValue) ptr);
}

void *iPtrSet_at(const iSet *d, size_t pos) {
    return (void *) iSet_at(d, pos);
}
