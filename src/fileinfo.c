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
#include "c_plus/path.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <unistd.h>
#include <dirent.h>

enum FileInfoFlags {
    exists_FileInfoFlag,
    directory_FileInfoFlag,
};

struct Impl_FileInfo {
    iObject object;
    iString *path;
    iTime lastModified;
    long size;
    enum FileInfoFlags flags;
};

iDefineClass(FileInfo)
iDefineObjectConstructionArgs(FileInfo, (const iString *path), path)

iFileInfo *newCStr_FileInfo(const char *path) {
    iString str; initCStr_String(&str, path);
    iFileInfo *info = new_FileInfo(&str);
    deinit_String(&str);
    return info;
}

void init_FileInfo(iFileInfo *d, const iString *path) {
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
        d->size = iFileInfoUnknownSize;
    }
}

static void initDirEntry_FileInfo_(iFileInfo *d, const iString *dirPath, struct dirent *ent) {
    iString entryName;
    initCStrN_String(&entryName, ent->d_name, ent->d_namlen);
    d->path = concat_Path(dirPath, &entryName);
    deinit_String(&entryName);

    d->flags = exists_FileInfoFlag;
    if (ent->d_type == DT_DIR) {
        d->flags |= directory_FileInfoFlag;
    }
    d->size = iFileInfoUnknownSize; // Unknown at this time.
}

void deinit_FileInfo(iFileInfo *d) {
    delete_String(d->path);
}

iBool exists_FileInfo(const iFileInfo *d) {
    return (d->flags & exists_FileInfoFlag) != 0;
}

const iString *path_FileInfo(const iFileInfo *d) {
    return d->path;
}

long size_FileInfo(const iFileInfo *d) {
    if (d->size < 0) {
        iConstCast(iFileInfo *, d)->size = fileSize_FileInfo(d->path);
    }
    return d->size;
}

iBool isDirectory_FileInfo(const iFileInfo *d) {
    return (d->flags & directory_FileInfoFlag) != 0;
}

iTime lastModified_FileInfo(const iFileInfo *d) {
    if (!isValid_Time(&d->lastModified)) {
        struct stat st;
        if (!stat(cstr_String(d->path), &st)) {
            iConstCast(iFileInfo *, d)->lastModified.ts = st.st_mtimespec;
        }
    }
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
    return iFileInfoUnknownSize;
}

//---------------------------------------------------------------------------------------

struct Impl_DirFileInfo {
    iObject object;
    const iFileInfo *dirInfo;
    DIR *fd;
    iFileInfo entry;
};

iDefineClass(DirFileInfo)
iDefineObjectConstructionArgs(DirFileInfo, (const iFileInfo *dir), dir)

static iBool readNextEntry_DirFileInfo_(iDirFileInfo *d) {
    deinit_FileInfo(&d->entry);
    struct dirent ent;
    struct dirent *result;
    readdir_r(d->fd, &ent, &result);
    iZap(d->entry);
    if (result == &ent) {
        initDirEntry_FileInfo_(&d->entry, path_FileInfo(d->dirInfo), result);
        return iTrue;
    }
    return iFalse;
}

void init_DirFileInfo(iDirFileInfo *d, const iFileInfo *dir) {
    iZap(d->entry);
    if (isDirectory_FileInfo(dir)) {
        d->fd = opendir(cstr_String(path_FileInfo(dir)));
        d->dirInfo = ref_Object(dir);
    }
    else {
        d->fd = NULL;
    }
}

void deinit_DirFileInfo(iDirFileInfo *d) {
    if (d->fd) {
        closedir(d->fd);
        d->fd = NULL;
    }
    deinit_FileInfo(&d->entry);
    deref_Object(d->dirInfo);
}

void init_DirFileInfoIterator(iDirFileInfoIterator *d, iDirFileInfo *info) {
    d->dir = info;
    next_DirFileInfoIterator(d);
}

void next_DirFileInfoIterator(iDirFileInfoIterator *d) {
    if (readNextEntry_DirFileInfo_(d->dir)) {
        d->value = &d->dir->entry;
    }
    else {
        d->value = NULL;
    }
}
