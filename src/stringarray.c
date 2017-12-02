/** @file stringarray.c  Array of strings.

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

#include "c_plus/stringarray.h"

#include <stdarg.h>

iDefineClass(StringArray)
iDefineObjectConstruction(StringArray)

void init_StringArray(iStringArray *d) {
    init_PtrArray(&d->strings);
}

void deinit_StringArray(iStringArray *d) {
    iForEach(PtrArray, i, &d->strings) {
        delete_String(i.ptr);
    }
    deinit_PtrArray(&d->strings);
}

iStringArray *newStrings_StringArray(const iString *str, ...) {
    iStringArray *d = new_StringArray();
    pushBack_StringArray(d, copy_String(str));
    va_list args;
    for (va_start(args, str);;) {
        str = va_arg(args, const iString *);
        if (!str) break;
        pushBack_StringArray(d, copy_String(str));
    }
    va_end(args);
    return d;
}

iStringArray *newStringsCStr_StringArray(const char *cstr, ...) {
    iStringArray *d = new_StringArray();
    pushBackCStr_StringArray(d, cstr);
    va_list args;
    for (va_start(args, cstr);;) {
        cstr = va_arg(args, const char *);
        if (!cstr) break;
        pushBackCStr_StringArray(d, cstr);
    }
    va_end(args);
    return d;
}

void resize_StringArray(iStringArray *d, size_t size) {
    const size_t oldSize = size_StringArray(d);
    if (size < oldSize) {
        for (size_t i = size; i < oldSize; ++i) {
            delete_String(at_PtrArray(&d->strings, i));
        }
    }
    else if (size > oldSize) {
        for (size_t i = size; i < oldSize; ++i) {
            set_PtrArray(&d->strings, i, new_String());
        }
    }
    resize_PtrArray(&d->strings, size);
}

iString *at_StringArray(iStringArray *d, size_t pos) {
    return at_PtrArray(&d->strings, pos);
}

const iString *constAt_StringArray(const iStringArray *d, size_t pos) {
    return at_PtrArray(&d->strings, pos);
}

void set_StringArray(iStringArray *d, size_t pos, const iString *str) {
    iAssert(pos < size_StringArray(d));
    delete_String(at_PtrArray(&d->strings, pos));
    set_PtrArray(&d->strings, pos, str);
}

void pushBack_StringArray(iStringArray *d, const iString *str) {
    pushBack_PtrArray(&d->strings, str);
}

void pushFront_StringArray(iStringArray *d, const iString *str) {
    pushFront_PtrArray(&d->strings, str);
}

void insert_StringArray(iStringArray *d, size_t pos, const iString *str) {
    insert_PtrArray(&d->strings, pos, str);
}

void setCStr_StringArray(iStringArray *d, size_t pos, const char *cstr) {
    set_StringArray(d, pos, newCStr_String(cstr));
}

void pushBackCStr_StringArray(iStringArray *d, const char *cstr) {
    pushBack_StringArray(d, newCStr_String(cstr));
}

void pushFrontCStr_StringArray(iStringArray *d, const char *cstr) {
    pushFront_StringArray(d, newCStr_String(cstr));
}

void insertCStr_StringArray(iStringArray *d, size_t pos, const char *cstr) {
    insert_StringArray(d, pos, newCStr_String(cstr));
}

void remove_StringArray(iStringArray *d, size_t pos) {
    void *str;
    if (take_PtrArray(&d->strings, pos, &str)) {
        delete_String(str);
    }
}

//---------------------------------------------------------------------------------------

void init_StringArrayIterator(iStringArrayIterator *d, iStringArray *array) {
    init_PtrArrayIterator(&d->iter, &array->strings);
}

void next_StringArrayIterator(iStringArrayIterator *d) {
    next_PtrArrayIterator(&d->iter);
}

void init_StringArrayConstIterator(iStringArrayConstIterator *d, const iStringArray *array) {
    init_PtrArrayConstIterator(&d->iter, &array->strings);
}

void next_StringArrayConstIterator(iStringArrayConstIterator *d) {
    next_PtrArrayConstIterator(&d->iter);
}
