#pragma once

/** @file c_plus/stream.h  Base class for streams.

Stream reads and writes serialized data. Stream is also a base class for more
specialized objects that provide access to a specific kind of data, for example a
native file or a memory buffer.

Streams are by default little-endian. The endianness can be changed at any point using
the setByteOrder_Stream() method.

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

#include "defs.h"
#include "object.h"

iDeclareType(Block)
iDeclareType(Stream)
iDeclareType(String)
iDeclareType(StringList)

iBeginDeclareClass(Stream)
    long        (*seek) (iStream *, long offset);
    size_t      (*read) (iStream *, size_t size, void *data_out);
    size_t      (*write)(iStream *, const void *data, size_t size);
    void        (*flush)(iStream *);
iEndDeclareClass(Stream)

enum iStreamByteOrder {
    littleEndian_StreamByteOrder, // the default
    bigEndian_StreamByteOrder,
};

struct Impl_Stream {
    iObject object;
    long size;
    long pos;
    int flags;
};

void        init_Stream         (iStream *);
void        deinit_Stream       (iStream *);

void        setByteOrder_Stream (iStream *, enum iStreamByteOrder byteOrder);
void        setSize_Stream      (iStream *, long size);

enum iStreamByteOrder byteOrder_Stream(const iStream *);

void        seek_Stream         (iStream *, long offset);
iBlock *    read_Stream         (iStream *, size_t size);
size_t      readData_Stream     (iStream *, size_t size, void *data_out);
size_t      readBlock_Stream    (iStream *, size_t size, iBlock *data_out);
iBlock *    readAll_Stream      (iStream *);

size_t      write_Stream        (iStream *, const iBlock *data);
size_t      writeData_Stream    (iStream *, const void *data, size_t size);

static inline void write8_Stream(iStream *d, int8_t value) { writeData_Stream(d, &value, 1); }

void        write16_Stream      (iStream *, int16_t value);
void        write32_Stream      (iStream *, int32_t value);
void        write64_Stream      (iStream *, int64_t value);

static inline void writef_Stream    (iStream *d, float value)   { int32_t buf; memcpy(&buf, &value, 4); write32_Stream(d, buf); }
static inline void writed_Stream    (iStream *d, double value)  { int64_t buf; memcpy(&buf, &value, 8); write64_Stream(d, buf); }

int8_t      read8_Stream        (iStream *);
int16_t     read16_Stream       (iStream *);
int32_t     read32_Stream       (iStream *);
int64_t     read64_Stream       (iStream *);

static inline uint8_t  readU8_Stream    (iStream *d) { return (uint8_t)  read8_Stream(d); }
static inline uint16_t readU16_Stream   (iStream *d) { return (uint16_t) read16_Stream(d); }
static inline uint32_t readU32_Stream   (iStream *d) { return (uint32_t) read32_Stream(d); }
static inline uint64_t readU64_Stream   (iStream *d) { return (uint64_t) read64_Stream(d); }

static inline float    readf_Stream     (iStream *d) { int32_t buf = read32_Stream(d); float  v; memcpy(&v, &buf, 4); return v; }
static inline double   readd_Stream     (iStream *d) { int64_t buf = read64_Stream(d); double v; memcpy(&v, &buf, 8); return v; }

void        flush_Stream        (iStream *);

iString *       readString_Stream   (iStream *);
iStringList *   readLines_Stream    (iStream *);

iAnyObject *    readObject_Stream   (iStream *, iAnyObject *object);
size_t          writeObject_Stream  (iStream *, const iAnyObject *object);

static inline long  size_Stream     (const iStream *d) { return d->size; }
static inline long  pos_Stream      (const iStream *d) { return d->pos; }
static inline iBool atEnd_Stream    (const iStream *d) { return d->pos == d->size; }
