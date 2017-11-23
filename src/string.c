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
#include <limits.h>
#include <wchar.h>

iString *new_String(void) {
    iString *d = calloc(sizeof(iString), 1);
    init_Block(&d->chars, 0);
    return d;
}

//iString *newUndefined_String(size_t len) {
//    iString *d = calloc(sizeof(iString), 1);
//    init_Block(&d->chars, len);
//    return d;
//}

iString *fromCStr_String(const char *cstr) {
    return fromCStrN_String(cstr, strlen(cstr));
}

iString *fromCStrN_String(const char *cstr, size_t len) {
    iString *d = calloc(sizeof(iString), 1);
    init_Block(&d->chars, len);
    setData_Block(&d->chars, cstr, len);
    return d;
}

iString *fromBlock_String(const iBlock *data) {
    iString *d = new_String();
    set_Block(&d->chars, data);
    return d;
}

const char *cstr_String(const iString *d) {
    return constData_Block(&d->chars);
}

void delete_String(iString *d) {
    deinit_Block(&d->chars);
    free(d);
}

void truncate_String(iString *d, size_t len) {

}

size_t size_String(const iString *d) {
    return size_Block(&d->chars);
}

iString *mid_String(const iString *d, size_t start, size_t count) {

}

void set_String(iString *d, const iString *other) {
    set_Block(&d->chars, &other->chars);
}

//---------------------------------------------------------------------------------------

static size_t decodeMultibyte_StringConstIterator_(iStringConstIterator *d) {
    wchar_t out = 0;
    const size_t result = mbrtowc(&out, d->pos, d->remaining, &d->mbs);
    if (result == 0) {
        // Finished.
        d->value = out;
    }
    else if (result == (size_t) -1 || result == (size_t) -2) {
        // Invalid or incomplete.
        d->value = 0;
        return 0;
    }
    else {
        d->value = out;
        d->remaining -= result;
    }
    return result;
}

static iBool stepBackward_StringConstIterator_(iStringConstIterator *d) {
    const char *first = constData_Block(&d->str->chars);
    if (d->pos == first) return iFalse;
    const char *prevStartsAt = d->pos - 1;
    for (int i = iMin(MB_LEN_MAX, d->remaining); i > 1; --i) {
        const iByte c = d->pos[-i];
        if (c >= 192 && c <= 253) {
            prevStartsAt = d->pos - i;
        }
    }
    d->remaining -= d->pos - prevStartsAt;
    d->pos = prevStartsAt;
    return iTrue;
}

void init_StringConstIterator(iStringConstIterator *d, const iString *str) {
    d->str = str;
    d->value = 0;
    d->pos = constData_Block(&str->chars);
    d->remaining = size_Block(&str->chars);
    iZap(d->mbs);
    // Decode the first character.
    d->pos += decodeMultibyte_StringConstIterator_(d);
}

void next_StringConstIterator(iStringConstIterator *d) {
    d->pos += decodeMultibyte_StringConstIterator_(d);
}

void init_StringReverseConstIterator(iStringConstIterator *d, const iString *str) {
    d->str = str;
    d->value = 0;
    d->pos = constEnd_Block(&str->chars);
    d->remaining = size_Block(&str->chars);
    iZap(d->mbs);
    // Decode the first (last) character.
    if (stepBackward_StringConstIterator_(d)) {
        decodeMultibyte_StringConstIterator_(d);
    }
}

void next_StringReverseConstIterator(iStringConstIterator *d) {
    if (stepBackward_StringConstIterator_(d)) {
        decodeMultibyte_StringConstIterator_(d);
    }
    else {
        d->value = 0;
    }
}

//---------------------------------------------------------------------------------------

int iCmpStrCase(const char *a, const char *b) {
    return strcasecmp(a, b);
}

int iCmpStrNCase(const char *a, const char *b, size_t len) {
    return strncasecmp(a, b, len);
}
