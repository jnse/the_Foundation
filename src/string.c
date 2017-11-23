/** @file string.c  Text strings.

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

#include "lite/string.h"

#include <stdlib.h>
#include <strings.h>

#define iCharSize(n) (sizeof(iChar) * (n))

#define constCharPtr_String_(d, pos) \
    (((const iChar *) constData_Block(&(d)->chars)) + pos)

iString *new_String(void) {
    return newUndefined_String(0);
}

iString *newUndefined_String(size_t len) {
    iString *d = calloc(sizeof(iString), 1);
    init_Block(&d->chars, iCharSize(len));
    return d;
}

iString *fromBlock_String(const iBlock *data) {
    iString *d = new_String();
    set_Block(&d->chars, data);
    return d;
}

void delete_String(iString *d) {
    deinit_Block(&d->chars);
    free(d);
}

void truncate_String(iString *d, size_t len) {
    truncate_Block(&d->chars, iCharSize(len));
}

size_t size_String(const iString *d) {
    return size_Block(&d->chars) / iCharSize(1);
}

iChar at_String(const iString *d, size_t pos) {
    iChar ch;
    memcpy(&ch, constCharPtr_String_(d, pos), iCharSize(1));
    return ch;
}

iString *mid_String(const iString *d, size_t start, size_t count) {
    iString *out = newUndefined_String(count);
    setData_Block(&out->chars, constCharPtr_String_(d, start), iCharSize(count));
    return out;
}

void set_String(iString *d, const iString *other) {
    set_Block(&d->chars, &other->chars);
}

//---------------------------------------------------------------------------------------

int iCmpStrCase(const char *a, const char *b) {
    return strcasecmp(a, b);
}

int iCmpStrNCase(const char *a, const char *b, size_t len) {
    return strncasecmp(a, b, len);
}
