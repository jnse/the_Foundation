#pragma once

/** @file buffer.h  Memory stream.

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

#include "stream.h"
#include "block.h"

#include <stdio.h>

typedef iStreamClass iBufferClass;

iDeclareType(Buffer)
iDeclareType(String)

enum iBufferMode {
    readWrite_BufferMode = 0,
    readOnly_BufferMode  = 0x1,
};

struct Impl_Buffer {
    iStream stream;
    iBlock block;
    iBlock *data;
    enum iBufferMode mode;
};

iDeclareObjectConstruction(Buffer)

iBool       open_Buffer     (iBuffer *, const iBlock *data);
iBool       openData_Buffer (iBuffer *, iBlock *data);
iBool       openEmpty_Buffer(iBuffer *);
void        close_Buffer    (iBuffer *);

const iBlock *data_Buffer   (const iBuffer *);

void        clear_Buffer    (iBuffer *);

static inline iBool isOpen_Buffer (const iBuffer *d) { return d->data != NULL; }
static inline int   mode_Buffer   (const iBuffer *d) { return d->mode ;}
static inline long  pos_Buffer    (const iBuffer *d) { return pos_Stream(&d->stream); }
static inline long  size_Buffer   (const iBuffer *d) { return size_Stream(&d->stream); }
static inline iBool atEnd_Buffer  (const iBuffer *d) { return atEnd_Stream(&(d)->stream); }

static inline size_t        writeData_Buffer    (iBuffer *d, const void *data, size_t size) { return writeData_Stream(&d->stream, data, size); }

static inline iBlock *      readAll_Buffer      (iBuffer *d) { return readAll_Stream(&d->stream); }
static inline iString *     readString_Buffer   (iBuffer *d) { return readString_Stream(&d->stream); }
static inline iStringList * readLines_Buffer    (iBuffer *d) { return readLines_Stream(&d->stream); }