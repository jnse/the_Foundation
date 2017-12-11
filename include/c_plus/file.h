#pragma once

/** @file file.h  File stream.

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

#include <stdio.h>

typedef iStreamClass iFileClass;

iDeclareType(File)
iDeclareType(String)

enum iFileMode {
    read_FileMode       = 0x1,
    readOnly_FileMode   = 0x1,
    write_FileMode      = 0x2,
    writeOnly_FileMode  = 0x2,
    append_FileMode     = 0x4 | write_FileMode,
    text_FileMode       = 0x8,

    readWrite_FileMode  = read_FileMode | write_FileMode,
};

struct Impl_File {
    iStream stream;
    iString *path;
    int flags;
    FILE *file;
};

iDeclareObjectConstructionArgs(File, const iString *path)

iFile *     newCStr_File    (const char *path);

iBool       open_File       (iFile *, int mode);
void        close_File      (iFile *);

long        seek_File       (iFile *, long offset);
size_t      read_File       (iFile *, size_t size, void *data_out);
size_t      write_File      (iFile *, const void *data, size_t size);
void        flush_File      (iFile *);

iString *   readString_File (iFile *d);

static inline iBool isOpen_File (const iFile *d) { return d->file != NULL; }
static inline int   flags_File  (const iFile *d) { return d->flags ;}
static inline long  pos_File    (const iFile *d) { return pos_Stream(&d->stream); }
static inline long  size_File   (const iFile *d) { return size_Stream(&d->stream); }
static inline iBool atEnd_File  (const iFile *d) { return atEnd_Stream(&(d)->stream); }
static inline const iString *path_File(const iFile *d) { return d->path; }

static inline iBlock *      readAll_File  (iFile *d) { return readAll_Stream(&d->stream); }
static inline iStringList * readLines_File(iFile *d) { return readLines_Stream(&d->stream); }
