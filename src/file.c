/** @file file.c  File stream.

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

#include "c_plus/file.h"
#include "c_plus/fileinfo.h"
#include "c_plus/string.h"

typedef iStreamClass iFileClass;

static iBeginDefineClass(File)
    .super  = &Class_Stream,
    .seek   = (long (*)(iStream *, long))                   seek_File,
    .read   = (size_t (*)(iStream *, size_t, void *))       read_File,
    .write  = (size_t (*)(iStream *, const void *, size_t)) write_File,
    .flush  = (void (*)(iStream *))                         flush_File,
iEndDefineClass(File)

iFile *new_File(const iString *path) {
    iFile *d = new_Object(&Class_File);
    init_Stream(&d->stream);
    d->path = copy_String(path);
    d->flags = readOnly_FileFlag;
    d->file = NULL;
    return d;
}

iFile *newCStr_File(const char *path) {
    iString str;
    initCStr_String(&str, path);
    iFile *d = new_File(&str);
    deinit_String(&str);
    return d;
}

void deinit_File(iFile *d) {
    if (d->file) {
        fclose(d->file);
    }
    delete_String(d->path);
}

iBool open_File(iFile *d, int flags) {
    if (isOpen_File(d)) return iFalse;
    setSize_Stream(&d->stream, fileSize_FileInfo(d->path));
    d->flags = (flags? flags : readOnly_FileFlag);
    char mode[4], *m = mode;
    if (d->flags & append_FileFlag) {
        *m++ = 'a';
        if (d->flags & read_FileFlag) { *m++ = '+'; }
    }
    else {
        if (d->flags & read_FileFlag) { *m++ = 'r'; }
        if (d->flags & write_FileFlag) {
            if (d->flags & read_FileFlag) { *m++ = '+'; }
            else { *m++ = 'w'; }
        }
    }
    if (d->flags & text_FileFlag) { *m++ = 't'; } else { *m++ = 'b'; }
    *m = 0;
    d->file = fopen(cstr_String(d->path), mode);
    return isOpen_File(d);
}

void close_File(iFile *d) {
    if (isOpen_File(d)) {
        fclose(d->file);
        d->file = NULL;
    }
}

long seek_File(iFile *d, long offset) {
    if (isOpen_File(d)) {
        fseek(d->file, offset, SEEK_SET);
        return ftell(d->file);
    }
    return pos_Stream(&d->stream);
}

size_t read_File(iFile *d, size_t size, void *data_out) {
    if (isOpen_File(d)) {
        return fread(data_out, size, 1, d->file);
    }
    return 0;
}

size_t write_File(iFile *d, const void *data, size_t size) {
    if (isOpen_File(d)) {
        return fwrite(data, size, 1, d->file);
    }
    return 0;
}

void flush_File(iFile *d) {
    if (isOpen_File(d)) {
        fflush(d->file);
    }
}
