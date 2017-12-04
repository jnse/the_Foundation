#pragma once

/** @file c_plus/fileinfo.h  File information.

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
#include "string.h"
#include "object.h"

iDeclareClass(FileInfo)
iDeclareClass(DirFileInfo)

iDeclareType(FileInfo)
iDeclareType(DirFileInfo)
iDeclareType(File)
iDeclareType(Time)

#define iFileInfoUnknownSize ((long) -1)

iFileInfo * new_FileInfo    (const iString *path);
iFileInfo * newCStr_FileInfo(const char *path);

void        init_FileInfo   (iFileInfo *, const iString *path);
void        deinit_FileInfo (iFileInfo *);

iBool       exists_FileInfo         (const iFileInfo *);
const iString * path_FileInfo       (const iFileInfo *);
long        size_FileInfo           (const iFileInfo *);
iBool       isDirectory_FileInfo    (const iFileInfo *);
iTime       lastModified_FileInfo   (const iFileInfo *);

#define     cstrPath_FileInfo(d)    cstr_String(path_FileInfo(d))

iBool       fileExists_FileInfo     (const iString *path);
iBool       fileExistsCStr_FileInfo (const char *path);
long        fileSize_FileInfo       (const iString *path);
long        fileSizeCStr_FileInfo   (const char *path);

iDirFileInfo *  directoryContents_FileInfo  (const iFileInfo *);

iFile *     open_FileInfo   (const iFileInfo *, int modeFlags);

//---------------------------------------------------------------------------------------

iDirFileInfo *  new_DirFileInfo         (const iString *path);
iDirFileInfo *  newCStr_DirFileInfo     (const char *path);

void            init_DirFileInfo        (iDirFileInfo *, const iString *path);
void            initInfo_DirFileInfo    (iDirFileInfo *, const iFileInfo *d);
void            deinit_DirFileInfo      (iDirFileInfo *);

/** @name Iterators */
///@{
iDeclareIterator(DirFileInfo, iDirFileInfo *)
struct IteratorImpl_DirFileInfo {
    const iFileInfo *value;
    iDirFileInfo *dir;
};
///@}
