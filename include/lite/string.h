#pragma once

/** @file lite/string.h  Wide-char text string with copy-on-write semantics.

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

iDeclareType(String);

struct Impl_String {
    iBlock chars;
};

iString *       new_String(void);
iString *       newUndefined_String(size_t len);
iString *       copy_String(const iString *);
void            delete_String(iString *);

#define         collect_String(d)   iCollectDel(d, delete_String)

iString *       fromLatin1_String(const char *cstr);
iString *       fromLatin1N_String(const char *cstr, size_t len);
iString *       fromUtf8_String(const char *utf8);
iString *       fromUtf8N_String(const char *utf8, size_t len);
iString *       fromBlock_String(const iBlock *data);

size_t          size_String(const iString *);
iChar           at_String(const iString *, size_t pos);
iString *       mid_String(const iString *, size_t start, size_t count);

void            set_String(iString *, const iString *other);
void            setLatin1_String(iString *, const char *cstr);
void            setUtf8_String(iString *, const char *utf8);
void            setChar_String(iString *, size_t pos, iChar ch);

size_t          indexOf_String(const iString *, const iString *other);
size_t          indexOfChar_String(const iString *, iChar ch);
size_t          lastIndexOf_String(const iString *, const iString *other);
size_t          lastIndexOfChar_String(const iString *, iChar ch);

void            truncate_String(iString *, size_t len);

#define         iCmpStr(a, b)       strcmp(a, b)
#define         iCmpStrN(a, b, len) strncmp(a, b, len)

int             iCmpStrCase (const char *a, const char *b);
int             iCmpStrNCase(const char *a, const char *b, size_t len);

