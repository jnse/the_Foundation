/** @file block.c  Byte array with copy-on-write semantics.

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

#include "lite/block.h"
#include "lite/atomic.h"
#include "lite/garbage.h"
#include "lite/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <strings.h>

struct Impl_BlockData {
    iAtomicInt refCount;
    char *data;
    size_t size;
    size_t allocSize;
};

static iBlockData emptyBlockData = { 1, "", 0, 1 };

static iBlockData *new_BlockData_(size_t size, size_t allocSize) {
    iBlockData *d = malloc(sizeof(iBlockData));
    set_Atomic(&d->refCount, 1);
    d->size = size;
    d->allocSize = iMax(size + 1, allocSize);
    d->data = malloc(d->allocSize);
    return d;
}

static iBlockData *duplicate_BlockData_(const iBlockData *d, size_t allocSize) {
    iBlockData *dupl = new_BlockData_(d->size, allocSize);
    memcpy(dupl->data, d->data, dupl->size + 1);
    return dupl;
}

static void deref_BlockData_(iBlockData *d) {
    const int oldRef = add_Atomic(&d->refCount, -1);
    if (oldRef == 1) {
        iAssert(d != &emptyBlockData);
        free(d->data);
        free(d);
    }
}

static void reserve_BlockData_(iBlockData *d, size_t size) {
    size++;
    if (d->allocSize >= size) return;
    iAssert(value_Atomic(&d->refCount) == 1);
    iAssert(d->allocSize > 0);
    while (d->allocSize < size) d->allocSize *= 2;
    d->data = realloc(d->data, d->allocSize);
}

//---------------------------------------------------------------------------------------

static void detach_Block_(iBlock *d, size_t allocSize) {
    if (value_Atomic(&d->i->refCount) > 1) {
        iBlockData *detached = duplicate_BlockData_(d->i, allocSize);
        deref_BlockData_(d->i);
        d->i = detached;
    }
    iAssert(value_Atomic(&d->i->refCount) == 1);
}

iBlock *new_Block(size_t size) {
    iBlock *d = malloc(sizeof(iBlock));
    init_Block(d, size);
    return d;
}

iBlock *newCStr_Block(const char *cstr) {
    iBlock *d = new_Block(strlen(cstr));
    memcpy(d->i->data, cstr, d->i->size);
    d->i->data[d->i->size] = 0;
    return d;
}

iBlock *newData_Block(const void *data, size_t size) {
    iBlock *d = new_Block(size);
    memcpy(d->i->data, data, size);
    d->i->data[d->i->size] = 0;
    return d;
}

iBlock *copy_Block(const iBlock *d) {
    if (d) {
        iBlock *dupl = malloc(sizeof(iBlock));
        dupl->i = d->i;
        add_Atomic(&d->i->refCount, 1);
        return dupl;
    }
    return NULL;
}

void delete_Block(iBlock *d) {
    deinit_Block(d);
    free(d);
}

void init_Block(iBlock *d, size_t size) {
    if (size == 0) {
        d->i = &emptyBlockData;
        add_Atomic(&emptyBlockData.refCount, 1);
    }
    else {
        d->i = new_BlockData_(size, 0);
    }
}

void deinit_Block(iBlock *d) {
    deref_BlockData_(d->i);
}

size_t size_Block(const iBlock *d) {
    return d->i->size;
}

char at_Block(const iBlock *d, size_t pos) {
    iAssert(pos < d->i->size);
    return d->i->data[pos];
}

char front_Block(const iBlock *d) {
    return d->i->data[0];
}

char back_Block(const iBlock *d) {
    return d->i->data[d->i->size - 1];
}

const void *constData_Block(const iBlock *d) {
    return d->i->data;
}

iBlock *mid_Block(const iBlock *d, size_t start, size_t count) {
    if (start >= d->i->size) {
        return new_Block(0);
    }
    const size_t midSize = iMin(count, d->i->size - start);
    iBlock *mid = new_Block(midSize);
    memcpy(mid->i->data, d->i->data + start, midSize);
    return mid;
}

void *data_Block(iBlock *d) {
    detach_Block_(d, 0);
    return d->i->data;
}

void clear_Block(iBlock *d) {
    deref_BlockData_(d->i);
    d->i = &emptyBlockData;
    add_Atomic(&emptyBlockData.refCount, 1);
}

void reserve_Block(iBlock *d, size_t reservedSize) {
    detach_Block_(d, reservedSize + 1);
    reserve_BlockData_(d->i, reservedSize);
}

void resize_Block(iBlock *d, size_t size) {
    reserve_Block(d, size);
    const size_t oldSize = d->i->size;
    d->i->size = size;
    memset(d->i->data + oldSize, 0, d->i->size - oldSize + 1);
}

void truncate_Block(iBlock *d, size_t size) {
    detach_Block_(d, 0);
    d->i->size = iMin(d->i->size, size);
}

void printf_Block(iBlock *d, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const int len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    reserve_Block(d, len);
    va_start(args, format);
    vsprintf(d->i->data, format, args);
    d->i->size = len;
    va_end(args);
}

void fill_Block(iBlock *d, char value) {
    detach_Block_(d, 0);
    memset(d->i->data, value, d->i->size);
}

void pushBack_Block(iBlock *d, char value) {
    reserve_Block(d, d->i->size + 1);
    d->i->data[d->i->size++] = value;
    d->i->data[d->i->size] = 0;
}

void popBack_Block(iBlock *d) {
    detach_Block_(d, 0);
    if (d->i->size > 0) {
        d->i->data[--d->i->size] = 0;
    }
}

void set_Block(iBlock *d, const iBlock *other) {
    deref_BlockData_(d->i);
    d->i = other->i;
    add_Atomic(&d->i->refCount, 1);
}

void setByte_Block(iBlock *d, size_t pos, char value) {
    detach_Block_(d, 0);
    iAssert(pos < d->i->size);
    d->i->data[pos] = value;
}

void setData_Block(iBlock *d, const void *data, size_t size) {
    reserve_Block(d, size);
    memcpy(d->i->data, data, size);
    d->i->data[size] = 0;
    d->i->size = size;
}

void setCStr_Block(iBlock *d, const char *cstr) {
    setData_Block(d, cstr, strlen(cstr));
}

void append_Block(iBlock *d, const iBlock *other) {
    appendData_Block(d, other->i->data, other->i->size);
}

void appendData_Block(iBlock *d, const void *data, size_t size) {
    reserve_Block(d, d->i->size + size);
    memcpy(d->i->data + d->i->size, data, size);
    d->i->size += size;
    d->i->data[d->i->size] = 0;
}

void appendCStr_Block(iBlock *d, const char *cstr) {
    appendData_Block(d, cstr, strlen(cstr));
}

iBlock *concat_Block(const iBlock *d, const iBlock *other) {
    iBlock *cat = new_Block(d->i->size + other->i->size);
    memcpy(cat->i->data,                  d->i->data,     d->i->size);
    memcpy(cat->i->data + d->i->size, other->i->data, other->i->size);
    cat->i->data[cat->i->size] = 0;
    return cat;
}

int cmp_Block(const iBlock *d, const iBlock *other) {
    return cmpData_Block(d, other->i->data, other->i->size);
}

int cmpCase_Block(const iBlock *d, const iBlock *other) {
    return iCmpStrCase(d->i->data, other->i->data);
}

int cmpCaseN_Block(const iBlock *d, const iBlock *other, size_t size) {
    return iCmpStrNCase(d->i->data, other->i->data, size);
}

int cmpData_Block(const iBlock *d, const char *data, size_t size) {
    return memcmp(d->i->data, data, iMin(size, d->i->size));
}

int cmpCStr_Block(const iBlock *d, const char *cstr) {
    return iCmpStr(d->i->data, cstr);
}

int cmpCStrN_Block(const iBlock *d, const char *cstr, size_t len) {
    return iCmpStrN(d->i->data, cstr, len);
}

int cmpCaseCStr_Block(const iBlock *d, const char *cstr) {
    return iCmpStrCase(d->i->data, cstr);
}

int cmpCaseCStrN_Block(const iBlock *d, const char *cstr, size_t len) {
    return iCmpStrNCase(d->i->data, cstr, len);
}