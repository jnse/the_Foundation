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

iPtrArray * iPtrArray_new(void)
{
    return iArray_new(sizeof(iPointer));
}

void iPtrArray_delete(iPtrArray *d)
{
    iArray_delete(d);
}

iPointer *iPtrArray_data(const iPtrArray *d)
{
    return iArray_data(d);
}

iPointer iPtrArray_at(const iPtrArray *d, size_t pos)
{
    iPointer ptr;
    memcpy(&ptr, iArray_at(d, pos), sizeof(iPointer));
    return ptr;
}

void iPtrArray_pushBack(iPtrArray *d, const iPointer ptr)
{
    iArray_pushBack(d, &ptr);
}

void iPtrArray_pushFront(iPtrArray *d, const iPointer ptr)
{
    iArray_pushFront(d, &ptr);
}

iBool iPtrArray_take(iPtrArray *d, size_t pos, iPointer *outPtr)
{
    return iArray_take(d, pos, &outPtr);
}

void iPtrArray_insert(iPtrArray *d, size_t pos, const iPointer ptr)
{
    iArray_insert(d, pos, &ptr);
}
