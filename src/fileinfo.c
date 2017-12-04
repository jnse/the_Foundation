/** @file fileinfo.c  File information.

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

#include "c_plus/fileinfo.h"
#include "c_plus/time.h"

#include <sys/stat.h>
#include <unistd.h>

enum FileInfoFlags {
    exists_FileInfoFlag,
    directory_FileInfoFlag,
};

struct Impl_FileInfo {
    iString *path;
    iTime lastModified;
    long size;
    int flags;
};

#if 0
static void init_FileInfo_(iFileInfo *d, const iString *path) {
    d->path = copy_String(path);
    d->flags = 0;
    struct stat st;
    if (!stat(cstr_String(d->path), &st)) {
        d->lastModified.ts = st.st_mtimespec;
        d->size = st.st_size;
        d->flags |= exists_FileInfoFlag;
        if (st.st_mode & S_IFDIR) d->flags |= directory_FileInfoFlag;
    }
    else {
        iZap(d->lastModified);
        d->size = 0;
    }
}

static void deinit_FileInfo_(iFileInfo *d) {
    delete_String(d->path);
}
#endif

iBool exists_FileInfo(const iFileInfo *d) {
    return (d->flags & exists_FileInfoFlag) != 0;
}

const iString *path_FileInfo(const iFileInfo *d) {
    return d->path;
}

long size_FileInfo(const iFileInfo *d) {
    return d->size;
}

iBool isDir_FileInfo(const iFileInfo *d) {
    return (d->flags & directory_FileInfoFlag) != 0;
}

iTime lastModified_FileInfo(const iFileInfo *d) {
    return d->lastModified;
}

iBool fileExists_FileInfo(const iString *path) {
    return fileExistsCStr_FileInfo(cstr_String(path));
}

iBool fileExistsCStr_FileInfo(const char *path) {
    return !access(path, R_OK);
}

long fileSize_FileInfo(const iString *path) {
    return fileSizeCStr_FileInfo(cstr_String(path));
}

long fileSizeCStr_FileInfo(const char *path) {
    struct stat st;
    if (!stat(path, &st)) {
        return st.st_size;
    }
    return 0;
}
