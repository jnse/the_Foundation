#pragma once

/** @file lite/string.h  UTF-8 text string with copy-on-write semantics.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>
All rights reserved.

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
#include "block.h"
#include <wchar.h>

typedef wchar_t iChar;

iDeclareType(String);

struct Impl_String {
    iBlock chars;
};

iString *       new_String(void);
//iString *       newUndefined_String(size_t len);
iString *       copy_String(const iString *);
void            delete_String(iString *);

#define         collect_String(d)   iCollectDel(d, delete_String)

iString *       fromCStr_String(const char *cstr);
iString *       fromCStrN_String(const char *cstr, size_t len);
iString *       fromBlock_String(const iBlock *data);

const char *    cstr_String(const iString *);
size_t          charCount_String(const iString *);
size_t          size_String(const iString *);
iString *       mid_String(const iString *, size_t start, size_t count);

void            set_String(iString *, const iString *other);

size_t          indexOf_String(const iString *, const iString *other);
size_t          indexOfChar_String(const iString *, iChar ch);
size_t          lastIndexOf_String(const iString *, const iString *other);
size_t          lastIndexOfChar_String(const iString *, iChar ch);

void            truncate_String(iString *, size_t len);

iDeclareConstIterator(String, const iString *);

struct ConstIterator_String {
    const iString *str;
    iChar value;
    const char *pos;
    size_t remaining;
    mbstate_t mbs;
};

#define         iCmpStr(a, b)       strcmp(a, b)
#define         iCmpStrN(a, b, len) strncmp(a, b, len)

int             iCmpStrCase (const char *a, const char *b);
int             iCmpStrNCase(const char *a, const char *b, size_t len);
