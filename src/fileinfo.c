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

#include "the_Foundation/fileinfo.h"
#include "the_Foundation/file.h"
#include "the_Foundation/time.h"
#include "the_Foundation/path.h"

#include <stdlib.h>
#if defined (iPlatformWindows)
#   include "platform/win32/msdirent.h"
#else
#   include <sys/stat.h>
#   include <sys/types.h>
#   if defined (iHaveSysDirent)
#       include <sys/dirent.h>
#   else
#       include <sys/dir.h>
#   endif
#   include <unistd.h>
#   include <dirent.h>
#endif

#if defined (iPlatformLinux) || defined (iPlatformMsys) || defined (iPlatformCygwin)
#   define st_mtimespec st_mtim
#endif

enum FileInfoFlags {
    exists_FileInfoFlag     = 1,
    directory_FileInfoFlag  = 2,
};

struct Impl_FileInfo {
    iObject object;
    iString *path;
    iTime lastModified;
    long size;
    uint32_t flags;
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
        #if defined (iPlatformWindows)
            d->lastModified.ts = (struct timespec){ .tv_sec = st.st_mtime };
        #else
            d->lastModified.ts = st.st_mtimespec;
        #endif
        d->size = st.st_size;
        d->flags |= exists_FileInfoFlag;
        if (st.st_mode & S_IFDIR) d->flags |= directory_FileInfoFlag;
    }
    else {
        iZap(d->lastModified);
        d->size = iFileInfoUnknownSize;
    }
}

static iBool initDirEntry_FileInfo_(iFileInfo *d, const iString *dirPath, struct dirent *ent) {
    iString entryName;
#if defined (iPlatformApple)
    initLocalCStrN_String(&entryName, ent->d_name, ent->d_namlen);
#else
    initLocalCStr_String(&entryName, ent->d_name);
#endif
    /* Check for ignored entries. */
    if (!cmp_String(&entryName, "..") || !cmp_String(&entryName, ".")) {
        deinit_String(&entryName);
        return iFalse;
    }
    d->path = concat_Path(dirPath, &entryName);
    clean_Path(d->path);
    deinit_String(&entryName);

    d->flags = exists_FileInfoFlag;
    if (ent->d_type == DT_DIR) {
        d->flags |= directory_FileInfoFlag;
    }
    d->size = iFileInfoUnknownSize; // Unknown at this time.
    return iTrue;
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
            #if defined (iPlatformWindows)
                iConstCast(iFileInfo *, d)->lastModified.ts = (struct timespec){ 
                    .tv_sec = st.st_mtime };
            #else
                iConstCast(iFileInfo *, d)->lastModified.ts = st.st_mtimespec;
            #endif
        }
    }
    return d->lastModified;
}

iDirFileInfo *directoryContents_FileInfo(const iFileInfo *fileInfo) {
    iDirFileInfo *d = iNew(DirFileInfo);
    initInfo_DirFileInfo(d, fileInfo);
    return d;
}

iFile *open_FileInfo(const iFileInfo *d, int modeFlags) {
    iFile *f = new_File(path_FileInfo(d));
    open_File(f, modeFlags);
    return f;
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

/*-------------------------------------------------------------------------------------*/

struct Impl_DirFileInfo {
    iObject object;
    const iFileInfo *dirInfo;
    DIR *fd;
    iFileInfo entry;
};

iDefineClass(DirFileInfo)
iDefineObjectConstructionArgs(DirFileInfo, (const iString *path), path)

iDirFileInfo *newCStr_DirFileInfo(const char *path) {
    iString str; initCStr_String(&str, path);
    iDirFileInfo *d = new_DirFileInfo(&str);
    deinit_String(&str);
    return d;
}

void init_DirFileInfo(iDirFileInfo *d, const iString *path) {
    iFileInfo *fileInfo = new_FileInfo(path);
    initInfo_DirFileInfo(d, fileInfo);
    iRelease(fileInfo);
}

void initInfo_DirFileInfo(iDirFileInfo *d, const iFileInfo *dir) {
    iZap(d->entry);
    if (isDirectory_FileInfo(dir)) {
        d->fd = opendir(cstr_String(path_FileInfo(dir)));
        d->dirInfo = ref_Object(dir);
    }
    else {
        d->fd = NULL;
        d->dirInfo = NULL;
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

static iBool readNextEntry_DirFileInfo_(iDirFileInfo *d) {
    deinit_FileInfo(&d->entry);
    for (;;) {
        struct dirent *result = NULL;
#if defined (iPlatformApple)
        struct dirent ent;
        readdir_r(d->fd, &ent, &result);
#else
        result = readdir(d->fd);
#endif
        iZap(d->entry);
        if (result) {
            if (!initDirEntry_FileInfo_(&d->entry, path_FileInfo(d->dirInfo), result)) {
                continue;
            }
            return iTrue;
        }
        return iFalse;
    }
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
