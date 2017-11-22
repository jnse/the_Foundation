/** @file ptrarray.c  Array of pointers.

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

#include "lite/ptrarray.h"

iPtrArray *new_PtrArray(void) {
    return new_Array(sizeof(iPointer));
}

void delete_PtrArray(iPtrArray *d) {
    delete_Array(d);
}

iPointer *data_PtrArray(const iPtrArray *d) {
    return data_Array(d);
}

iPointer at_PtrArray(const iPtrArray *d, size_t pos) {
    iPointer ptr;
    memcpy(&ptr, at_Array(d, pos), sizeof(iPointer));
    return ptr;
}

void pushBack_PtrArray(iPtrArray *d, const iPointer ptr) {
    pushBack_Array(d, &ptr);
}

void pushFront_PtrArray(iPtrArray *d, const iPointer ptr) {
    pushFront_Array(d, &ptr);
}

iBool take_PtrArray(iPtrArray *d, size_t pos, iPointer *outPtr) {
    return take_Array(d, pos, &outPtr);
}

void insert_PtrArray(iPtrArray *d, size_t pos, const iPointer ptr) {
    insert_Array(d, pos, &ptr);
}
