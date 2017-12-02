#pragma once

/** @file c_plus/stringarray.h  Array of strings.

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

#include "ptrarray.h"
#include "object.h"
#include "string.h"

iDeclareClass(StringArray)
iDeclareType(StringArray)

struct Impl_StringArray {
    iObject object;
    iPtrArray strings;
};

iStringArray *  new_StringArray     (void);

iStringArray *  newStrings_StringArray      (const iString *, ...);
iStringArray *  newStringsCStr_StringArray  (const char *, ...);

void            init_StringArray    (iStringArray *);
void            deinit_StringArray  (iStringArray *);

#define         isEmpty_StringArray(d)      isEmpty_Array(&(d)->strings)
#define         size_StringArray(d)         size_Array(&(d)->strings)
#define         data_StringArray(d)         ((iString **) data_PtrArray(&(d)->strings))
#define         constData_StringArray(d)    ((const iString **) constData_PtrArray(&(d)->strings))

/**
 * Resizes the string array. When growing the array, empty strings are used for filling
 * new array positions.
 *
 * @param size  New size of the array.
 */
void            resize_StringArray      (iStringArray *, size_t size);

iString *       at_StringArray          (iStringArray *, size_t pos);
const iString * constAt_StringArray     (const iStringArray *, size_t pos);

void            set_StringArray         (iStringArray *, size_t pos, const iString *str);
void            pushBack_StringArray    (iStringArray *, const iString *str);
void            pushFront_StringArray   (iStringArray *, const iString *str);
void            insert_StringArray      (iStringArray *, size_t pos, const iString *str);

#define         cstrAt_StringArray(d, pos)  cstr_String(at_StringArray(d, pos))

void            setCStr_StringArray         (iStringArray *, size_t pos, const char *cstr);
void            pushBackCStr_StringArray    (iStringArray *, const char *cstr);
void            pushFrontCStr_StringArray   (iStringArray *, const char *cstr);
void            insertCStr_StringArray      (iStringArray *, size_t pos, const char *cstr);

void            remove_StringArray      (iStringArray *, size_t pos);

/** @name Iterators */
///@{
iDeclareIterator(StringArray, iStringArray *)
iDeclareConstIterator(StringArray, const iStringArray *)

#define index_StringArrayIterator(d)        index_ArrayIterator(&(d)->iter.iter)
#define index_StringArrayConstIterator(d)   index_ArrayConstIterator(&(d)->iter.iter)

struct IteratorImpl_StringArray {
    union {
        iString **value;
        iPtrArrayIterator iter;
    };
};

struct ConstIteratorImpl_StringArray {
    union {
        const iString * const *value;
        iPtrArrayConstIterator iter;
    };
};
///@}
