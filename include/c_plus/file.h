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

iDeclareType(File)
iDeclareType(String)

enum iFileFlags {
    read_FileFlag       = 0x1,
    readOnly_FileFlag   = 0x1,
    write_FileFlag      = 0x2,
    writeOnly_FileFlag  = 0x2,
    append_FileFlag     = 0x4 | write_FileFlag,
    text_FileFlag       = 0x8,

    readWrite_FileFlag  = read_FileFlag | write_FileFlag,
};

struct Impl_File {
    iStream stream;
    iString *path;
    int flags;
    FILE *file;
};

iFile *     new_File        (const iString *path);
iFile *     newCStr_File    (const char *path);

void        deinit_File     (iFile *);

iBool       open_File       (iFile *, int flags);
void        close_File      (iFile *);

#define     isOpen_File(d)  ((d)->file != NULL)

#define     flags_File(d)   ((d)->flags)
#define     pos_File(d)     pos_Stream(&(d)->stream)
#define     size_File(d)    size_Stream(&(d)->stream)
#define     atEnd_File(d)   atEnd_Stream(&(d)->stream)

long        seek_File       (iFile *, long offset);
size_t      read_File       (iFile *, size_t size, void *data_out);
size_t      write_File      (iFile *, const void *data, size_t size);
void        flush_File      (iFile *);
