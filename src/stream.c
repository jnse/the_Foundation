/** @file stream.c

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

#include "c_plus/stream.h"
#include "c_plus/block.h"
#include "c_plus/stringlist.h"
#include "c_plus/buffer.h"

iDefineClass(Stream)

#define class_Stream(d)         ((const iStreamClass *) (d)->object.class)

enum iStreamFlags {
    bigEndianByteOrder_StreamFlag = 1,
};

void init_Stream(iStream *d) {
    d->size = 0;
    d->pos = 0;
    d->flags = 0; // little-endian
}

void deinit_Stream(iStream *d) {
    iUnused(d);
}

void setByteOrder_Stream(iStream *d, enum iStreamByteOrder byteOrder) {
    iChangeFlags(d->flags, bigEndianByteOrder_StreamFlag, byteOrder == bigEndian_StreamByteOrder);
}

void setSize_Stream(iStream *d, long size) {
    d->size = size;
}

void seek_Stream(iStream *d, long offset) {
    d->pos = class_Stream(d)->seek(d, offset);
}

iBlock *read_Stream(iStream *d, size_t size) {
    iBlock *data = new_Block(0);
    readBlock_Stream(d, size, data);
    return data;
}

static inline size_t readData_Stream_(iStream *d, size_t size, void *data_out) {
    const size_t readSize = class_Stream(d)->read(d, size, data_out);
    d->pos += readSize;
    d->size = iMax(d->size, d->pos); // update successfully read size
    return readSize;
}

size_t readBlock_Stream(iStream *d, size_t size, iBlock *data_out) {
    resize_Block(data_out, size);
    const size_t readSize = readData_Stream_(d, size, data_Block(data_out));
    truncate_Block(data_out, readSize);
    return readSize;
}

iBlock *readAll_Stream(iStream *d) {
    iBlock *data = new_Block(0);
    iBlock *chunk = new_Block(0);
    for (;;) {
        size_t readSize = readBlock_Stream(d, 128 * 1024, chunk);
        if (!readSize) break;
        append_Block(data, chunk);
    }
    delete_Block(chunk);
    return data;
}

size_t write_Stream(iStream *d, const iBlock *data) {
    return writeData_Stream(d, constData_Block(data), size_Block(data));
}

size_t writeData_Stream(iStream *d, const void *data, size_t size) {
    const size_t n = class_Stream(d)->write(d, data, size);
    d->pos += n;
    d->size = iMax(d->pos, d->size);
    return n;
}

iStringList *readLines_Stream(iStream *d) {
    iBlock *data = readAll_Stream(d);
    iStringList *lines = split_String((iString *) data, "\n");
    delete_Block(data);
    return lines;
}

void flush_Stream(iStream *d) {
    class_Stream(d)->flush(d);
}

iString *readString_Stream(iStream *d) {
    iBlock *chars = readAll_Stream(d);
    iString *str = newBlock_String(chars);
    delete_Block(chars);
    return str;
}

iAnyObject *readObject_Stream(iStream *d, iAnyObject *object) {
    iAssert(((iObject *) object)->class->deserialize != NULL);
    ((iObject *) object)->class->deserialize(object, d);
    return object;
}

size_t writeObject_Stream(iStream *d, const iAnyObject *object) {
    iAssert(((const iObject *) object)->class->serialize != NULL);
    const long start = d->pos;
    ((const iObject *) object)->class->serialize(object, d);
    iAssert(d->pos >= start);
    return d->pos - start;
}

void        write16_Stream      (iStream *, int16_t value);

void        write32_Stream      (iStream *, int32_t value);

void        write64_Stream      (iStream *, int64_t value);

int8_t read8_Stream(iStream *d) {
    int8_t value = 0;
    readData_Stream_(d, 1, &value);
    return value;
}

int16_t     read16_Stream       (iStream *);

int32_t     read32_Stream       (iStream *);

int64_t     read64_Stream       (iStream *);
