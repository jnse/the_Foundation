/** @file block.c  Byte array with copy-on-write semantics.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>

@par License

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

#include "c_plus/block.h"
#include "c_plus/atomic.h"
#include "c_plus/garbage.h"
#include "c_plus/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <strings.h>
#if defined (iHaveZlib)
#   include <zlib.h>
#endif

/// @todo Needs a ref-counting mutex.
static iBlockData emptyBlockData = {
    .refCount = 1,
    .data = "",
    .size = 0,
    .allocSize = 1,
};

static iBlockData *new_BlockData_(size_t size, size_t allocSize) {
    iBlockData *d = malloc(sizeof(iBlockData));
    d->refCount = 1;
    d->size = size;
    d->allocSize = iMax(size + 1, allocSize);
    d->data = malloc(d->allocSize);
    return d;
}

static iBlockData *newPrealloc_BlockData_(void *data, size_t size) {
    iBlockData *d = malloc(sizeof(iBlockData));
    d->refCount = 1;
    d->size = size;
    d->allocSize = size;
    d->data = data;
    return d;
}

static iBlockData *duplicate_BlockData_(const iBlockData *d, size_t allocSize) {
    iBlockData *dupl = new_BlockData_(d->size, allocSize);
    memcpy(dupl->data, d->data, iMin(d->allocSize, dupl->size + 1));
    return dupl;
}

static void deref_BlockData_(iBlockData *d) {
    if (--d->refCount == 0) {
        iAssert(d != &emptyBlockData);
        free(d->data);
        free(d);
    }
}

static void reserve_BlockData_(iBlockData *d, size_t size) {
    size++;
    if (d->allocSize >= size) return;
    iAssert(d->refCount == 1);
    iAssert(d->allocSize > 0);
    while (d->allocSize < size) d->allocSize *= 2;
    d->data = realloc(d->data, d->allocSize);
}

static void memcpyFrom_Block_(iBlock *d, const void *data, size_t size) {
    memcpy(d->i->data, data, size);
    d->i->data[size] = 0;
    d->i->size = size;
}

static void detach_Block_(iBlock *d, size_t allocSize) {
    if (d->i->refCount > 1) {
        iBlockData *detached = duplicate_BlockData_(d->i, allocSize);
        deref_BlockData_(d->i);
        d->i = detached;
    }
    iAssert(d->i->refCount == 1);
}

//---------------------------------------------------------------------------------------

iDefineTypeConstructionArgs(Block, (size_t size), size)

iBlock *newCStr_Block(const char *cstr) {
    iBlock *d = new_Block(strlen(cstr));
    memcpyFrom_Block_(d, cstr, d->i->size);
    return d;
}

iBlock *newData_Block(const void *data, size_t size) {
    iBlock *d = new_Block(size);
    memcpyFrom_Block_(d, data, size);
    return d;
}

iBlock *newPrealloc_Block(void *data, size_t size) {
    iBlock *d = iMalloc(Block);
    initPrealloc_Block(d, data, size);
    return d;
}

iBlock *copy_Block(const iBlock *d) {
    if (d) {
        iBlock *dupl = malloc(sizeof(iBlock));
        initCopy_Block(dupl, d);
        return dupl;
    }
    return NULL;
}

void init_Block(iBlock *d, size_t size) {
    if (size == 0) {
        d->i = &emptyBlockData;
        emptyBlockData.refCount++;
    }
    else {
        d->i = new_BlockData_(size, 0);
    }
}

void initData_Block(iBlock *d, const void *data, size_t size) {
    if (size > 0) {
        d->i = new_BlockData_(size, 0);
        memcpyFrom_Block_(d, data, size);
    }
    else {
        init_Block(d, size);
    }
}

void initPrealloc_Block(iBlock *d, void *data, size_t size) {
    d->i = newPrealloc_BlockData_(data, size);
}

void initCopy_Block(iBlock *d, const iBlock *other) {
    d->i = other->i;
    d->i->refCount++;
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

const char *constBegin_Block(const iBlock *d) {
    return d->i->data;
}

const char *constEnd_Block(const iBlock *d) {
    return d->i->data + d->i->size;
}

iBlock *mid_Block(const iBlock *d, size_t start, size_t count) {
    if (start >= d->i->size) {
        return new_Block(0);
    }
    const size_t midSize = iMin(count, d->i->size - start);
    iBlock *mid = new_Block(midSize);
    memcpyFrom_Block_(mid, d->i->data + start, midSize);
    return mid;
}

void *data_Block(iBlock *d) {
    detach_Block_(d, 0);
    return d->i->data;
}

void clear_Block(iBlock *d) {
    deref_BlockData_(d->i);
    d->i = &emptyBlockData;
    emptyBlockData.refCount++;
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
    d->i->data[d->i->size] = 0;
}

void remove_Block(iBlock *d, size_t start, size_t count) {
    detach_Block_(d, 0);
    iAssert(start <= d->i->size);
    if (start + count > d->i->size) {
        count = d->i->size - start;
    }
    const size_t remainder = d->i->size - start - count;
    if (remainder > 0) {
        memmove(d->i->data + start, d->i->data + start + count, remainder);
    }
    d->i->size -= count;
    d->i->data[d->i->size] = 0;
}

void printf_Block(iBlock *d, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf_Block(d, format, args);
    va_end(args);
}

void vprintf_Block(iBlock *d, const char *format, va_list args) {
    va_list args2;
    va_copy(args2, args);
    const int len = vsnprintf(NULL, 0, format, args);
    reserve_Block(d, len);
    vsprintf(d->i->data, format, args2);
    d->i->size = len;
    va_end(args2);
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
    d->i->refCount++;
}

void setByte_Block(iBlock *d, size_t pos, char value) {
    detach_Block_(d, 0);
    iAssert(pos < d->i->size);
    d->i->data[pos] = value;
}

void setData_Block(iBlock *d, const void *data, size_t size) {
    reserve_Block(d, size);
    memcpyFrom_Block_(d, data, size);
}

void setSubData_Block(iBlock *d, size_t pos, const void *data, size_t size) {
    reserve_Block(d, pos + size);
    iAssert(pos <= d->i->size);
    memcpy(d->i->data + pos, data, size);
    d->i->size = iMax(d->i->size, pos + size);
    if (d->i->size == pos + size) {
        d->i->data[d->i->size] = 0;
    }
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

uint32_t crc32_Block(const iBlock *d) {
    return iCrc32(d->i->data, d->i->size);
}

//---------------------------------------------------------------------------------------
#if defined (iHaveZlib)

iDeclareType(ZStream)

struct Impl_ZStream {
    z_stream stream;
    iBlock *out;
};

static void init_ZStream_(iZStream *d, const iBlock *in, iBlock *out) {
    d->out = out;
    iZap(d->stream);
    d->stream.avail_in  = in->i->size;
    d->stream.next_in   = (Bytef *) in->i->data;
    d->stream.avail_out = out->i->size;
    d->stream.next_out  = (Bytef *) out->i->data;
}

static iBool process_ZStream_(iZStream *d, int (*process)(z_streamp, int)) {
    int opts = Z_NO_FLUSH;
    for (;;) {
        int rc = process(&d->stream, opts);
        if (rc == Z_STREAM_END) {
            break;
        }
        else if (rc != Z_OK && rc != Z_BUF_ERROR) {
            // Something went wrong.
            return iFalse;
        }
        if (d->stream.avail_out == 0) {
            // Allocate more room.
            const size_t oldSize = size_Block(d->out);
            resize_Block(d->out, oldSize * 2);
            d->stream.next_out = (Bytef *) d->out->i->data + oldSize;
            d->stream.avail_out = size_Block(d->out) - oldSize;
        }
        if (d->stream.avail_in == 0) {
            opts = Z_FINISH;
        }
    }
    truncate_Block(d->out, size_Block(d->out) - d->stream.avail_out);
    return iTrue;
}

iBlock *compressLevel_Block(const iBlock *d, int level) {
    iBlock *out = new_Block(1024);
    iZStream z;
    init_ZStream_(&z, d, out);
    /*
     * The deflation is done in raw mode. From zlib documentation:
     *
     * "windowBits can also be –8..–15 for raw deflate. In this case, -windowBits
     * determines the window size. deflate() will then generate raw deflate data with no
     * zlib header or trailer, and will not compute an adler32 check value."
     */
    if (deflateInit2(&z.stream, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) == Z_OK) {
        if (!process_ZStream_(&z, deflate)) {
            clear_Block(out);
        }
    }
    deflateEnd(&z.stream);
    return out;
}

iBlock *decompress_Block(const iBlock *d) {
    iBlock *out = new_Block(1024);
    iZStream z;
    init_ZStream_(&z, d, out);
    if (inflateInit2(&z.stream, -MAX_WBITS) == Z_OK) {
        if (!process_ZStream_(&z, inflate)) {
            clear_Block(out);
        }
    }
    inflateEnd(&z.stream);
    return out;
}

#endif // HaveZlib
