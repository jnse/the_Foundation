/** @file path.c  File path manipulation.

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

#include "the_Foundation/path.h"
#include "the_Foundation/string.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined (iPlatformWindows)
#   include <direct.h>
#   define mkdir(path, attr) _mkdir(path)
#endif

iString *cwd_Path(void) {
    char *cwd = getcwd(NULL, 0);
    if (cwd) {
        iBlock block;
        const size_t len = strlen(cwd);
        initPrealloc_Block(&block, cwd, len, len + 1);
        iString *d = newBlock_String(&block);
        deinit_Block(&block);
        return d;
    }
    return new_String();
}

iBool setCwd_Path(const iString *path) {
    return !chdir(cstr_String(path));
}

iBool isAbsolute_Path(const iString *d) {
    return startsWith_String(d, iPathSeparator) || startsWith_String(d, "~");
}

iString *makeAbsolute_Path(const iString *d) {
    if (isAbsolute_Path(d)) {
        return copy_String(d);
    }
    iString *abs = cwd_Path();
    append_Path(abs, d);
    clean_Path(abs);
    return abs;
}

#define iPathMaxSegments 128

static iBool splitSegments_Path_(const iRangecc *path, iRangecc *segments,
                                 size_t *count, iBool *changed) {
    iRangecc seg = iNullRange;
    while (nextSplit_Rangecc(path, iPathSeparator, &seg)) {
        if (*count > 0 && size_Range(&seg) == 0) {
            // Skip repeated slashes.
            *changed = iTrue;
            continue;
        }
        if (*count == 0 && !iCmpStrRange(&seg, "~")) {
            const char *home = getenv("HOME");
            if (home && iCmpStrN(home, "~", 1)) {
                if (!splitSegments_Path_(&(iRangecc){ home, home + strlen(home) },
                                         segments, count, changed)) {
                    return iFalse;
                }
                *changed = iTrue;
                continue;
            }
        }
        if (!iCmpStrRange(&seg, ".")) {
            *changed = iTrue;
            continue; // No change in directory.
        }
        if (!iCmpStrRange(&seg, "..")) {
            if (*count > 0 && iCmpStrRange(&segments[*count - 1], "..")) {
                (*count)--; // Go up a directory.
                *changed = iTrue;
                continue;
            }
        }
        if (*count == iPathMaxSegments) {
            iAssert(*count < iPathMaxSegments);
            return iFalse; // Couldn't clean it.
        }
        segments[(*count)++] = seg;
    }
    return iTrue;
}

void clean_Path(iString *d) {
    if (isEmpty_String(d)) return;
    iRangecc segments[iPathMaxSegments];
    size_t count = 0;
    iBool changed = iFalse;
    splitSegments_Path_(&range_String(d), segments, &count, &changed);
    // Recompose the remaining segments.
    if (changed) {
        if (count == 0) {
            set_String(d, &iStringLiteral("."));
            return;
        }
        iString cleaned;
        init_String(&cleaned);
        for (size_t i = 0; i < count; ++i) {
            if (i != 0 || isAbsolute_Path(d)) {
                appendCStr_String(&cleaned, iPathSeparator);
            }
            appendRange_String(&cleaned, segments + i);
        }
        set_String(d, &cleaned);
        deinit_String(&cleaned);
    }
}

void append_Path(iString *d, const iString *path) {
    if (isAbsolute_Path(path)) {
        set_String(d, path);
    }
    else {
        if (!endsWith_String(d, iPathSeparator)) {
            appendCStr_String(d, iPathSeparator);
        }
        append_String(d, path);
    }
}

iString *concat_Path(const iString *d, const iString *path) {
    iString *base = copy_String(d);
    append_Path(base, path);
    return base;
}

iBool mkdir_Path(const iString *path) {
    return mkdir(cstr_String(path), 0755) == 0;
}

iBool rmdir_Path(const iString *path) {
    return rmdir(cstr_String(path)) == 0;
}
