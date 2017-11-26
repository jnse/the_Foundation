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

#include "c_plus/ptrarray.h"

#include <stdarg.h>

iPtrArray *new_PtrArray(void) {
    return new_Array(sizeof(void *));
}

iPtrArray *newPointers_PtrArray(void *ptr, ...) {
    iPtrArray *d = new_PtrArray();
    pushBack_PtrArray(d, ptr);
    va_list args;
    va_start(args, ptr);
    void *next;
    while ((next = va_arg(args, void *)) != NULL) {
        pushBack_PtrArray(d, next);
    }
    va_end(args);
    return d;
}

void delete_PtrArray(iPtrArray *d) {
    delete_Array(d);
}

void **data_PtrArray(const iPtrArray *d) {
    return data_Array(d);
}

void *at_PtrArray(const iPtrArray *d, size_t pos) {
    return *(void **) at_Array(d, pos);
}

void set_PtrArray(iPtrArray *d, size_t pos, const void *ptr) {
    set_Array(d, pos, &ptr);
}

void pushBack_PtrArray(iPtrArray *d, const void *ptr) {
    pushBack_Array(d, &ptr);
}

void pushFront_PtrArray(iPtrArray *d, const void *ptr) {
    pushFront_Array(d, &ptr);
}

iBool take_PtrArray(iPtrArray *d, size_t pos, void **outPtr) {
    return take_Array(d, pos, &outPtr);
}

void insert_PtrArray(iPtrArray *d, size_t pos, const void *ptr) {
    insert_Array(d, pos, &ptr);
}

//---------------------------------------------------------------------------------------

void init_PtrArrayIterator(iPtrArrayIterator *d, iPtrArray *array) {
    d->array = array;
    d->pos = 0;
    d->value = (!isEmpty_PtrArray(array)? at_Array(array, 0) : NULL);
    d->ptr = (d->value? *d->value : NULL);
}

void next_PtrArrayIterator(iPtrArrayIterator *d) {
    if (++d->pos < size_Array(d->array)) {
        d->value = at_Array(d->array, d->pos);
        d->ptr = *d->value;
    }
    else {
        d->value = d->ptr = NULL;
    }
}

void init_PtrArrayConstIterator(iPtrArrayConstIterator *d, const iPtrArray *array) {
    d->array = array;
    d->value = (!isEmpty_Array(array)? front_Array(array) : NULL); // element
    d->ptr = (d->value? *d->value : NULL);
}

void next_PtrArrayConstIterator(iPtrArrayConstIterator *d) {
    if (++d->value < (const void * const *) constEnd_Array(d->array)) {
        d->ptr = *d->value;
    }
    else {
        d->value = NULL;
        d->ptr = NULL;
    }
}
