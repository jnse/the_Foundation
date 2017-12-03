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

iDefineClass(Stream)

#define class_Stream(d)         ((const iStreamClass *) (d)->object.class)

void init_Stream(iStream *d) {
    d->size = 0;
    d->pos = 0;
}

void deinit_Stream(iStream *d) {
    iUnused(d);
}

void seek_Stream(iStream *d, int64_t offset) {
    d->pos = class_Stream(d)->seek(d, offset);
}

iBlock *read_Stream(iStream *d, size_t size) {
    iBlock *data = new_Block(0);
    readBlock_Stream(d, size, data);
    return data;
}

size_t readBlock_Stream(iStream *d, size_t size, iBlock *data_out) {
    resize_Block(data_out, size);
    const size_t readSize = class_Stream(d)->read(d, size, data_Block(data_out));
    truncate_Block(data_out, readSize);
    d->pos += readSize;
    return readSize;
}

iBlock *readAll_Stream(iStream *d) {
    return read_Stream(d, d->size - d->pos);
}

size_t write_Stream(iStream *d, const iBlock *data) {
    return class_Stream(d)->write(d, constData_Block(data), size_Block(data));
}

size_t writeData_Stream(iStream *d, const void *data, size_t size) {
    return class_Stream(d)->write(d, data, size);
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
