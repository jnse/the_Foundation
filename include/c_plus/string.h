#pragma once

/** @file c_plus/string.h  Text string with multibyte characters (copy-on-write).

String is derived from Block, and contains text with multibyte characters. When
iterating a string, the multibyte characters are converted to wchar_t according to the
current locale.

String uses copy-on-write semantics (thanks to Block), so making copies is very
efficient. Conversions between String and Block are also trivial, and can be done
without duplicating the content. In fact, a pointer to a Block can be simply casted to
a String pointer and vice versa.

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
#include "block.h"

#include <limits.h>
#include <wchar.h>

typedef wchar_t iChar;

iDeclareType(String)
iDeclareType(StringList)
iDeclareType(StringComparison)
iDeclareType(MultibyteChar)

struct Impl_String {
    iBlock chars;
};

#define iStringLiteral(str)     (iString){ iBlockLiteral(str, strlen(str), strlen(str) + 1) }

iDeclareTypeConstruction(String)

iString *       newCStr_String  (const char *cstr);
iString *       newCStrN_String (const char *cstr, size_t n);
iString *       newWide_String  (const iChar *wstr);
iString *       newWideN_String (const iChar *wstr, size_t n);
iString *       newBlock_String (const iBlock *data);
iString *       copy_String     (const iString *);

static inline iString *newRange_String(const iRangecc *range) { return newCStrN_String(range->start, size_Range(range)); }

void            init_String     (iString *);
void            initCStr_String (iString *, const char *cstr);
void            initCStrN_String(iString *, const char *cstr, size_t n);
void            initWide_String (iString *, const iChar *wstr);
void            initWideN_String(iString *, const iChar *wstr, size_t n);
void            initBlock_String(iString *, const iBlock *chars);
void            initCopy_String (iString *, const iString *other);

const char *    cstr_String     (const iString *);
size_t          length_String   (const iString *);
size_t          size_String     (const iString *);
iString *       mid_String      (const iString *, size_t start, size_t count);
iString *       toUpper_String  (const iString *);
iString *       toLower_String  (const iString *);

#define         range_String(d) (iRangecc){ constData_Block(&d->chars), constEnd_Block(&d->chars) }

static inline iBool         isEmpty_String   (const iString *d) { return size_String(d) == 0; }
static inline const char *  constBegin_String(const iString *d) { return cstr_String(d); }
static inline const char *  constEnd_String  (const iString *d) { return cstr_String(d) + size_String(d); }

int             cmpSc_String        (const iString *, const char *cstr, const iStringComparison *);
int             cmpNSc_String       (const iString *, const char *cstr, size_t n, const iStringComparison *);

#define         cmp_String(d, cstr)             cmpSc_String(d, cstr, &iCaseSensitive)
#define         cmpCase_String(d, cstr)         cmpSc_String(d, cstr, &iCaseInsensitive)
#define         cmpString_String(d, s)          cmpSc_String(d, cstr_String(s), &iCaseSensitive)
#define         cmpStringCase_String(d, s)      cmpSc_String(d, cstr_String(s), &iCaseInsensitive)

iBool           startsWithSc_String (const iString *, const char *cstr, const iStringComparison *);
iBool           endsWithSc_String   (const iString *, const char *cstr, const iStringComparison *);

#define         startsWith_String(d, cstr)      startsWithSc_String(d, cstr, &iCaseSensitive)
#define         startsWithCase_String(d, cstr)  startsWithSc_String(d, cstr, &iCaseInsensitive)
#define         endsWith_String(d, cstr)        endsWithSc_String  (d, cstr, &iCaseSensitive)
#define         endsWithCase_String(d, cstr)    endsWithSc_String  (d, cstr, &iCaseInsensitive)

size_t          indexOf_String              (const iString *, iChar ch);
size_t          indexOfCStr_String          (const iString *, const char *cstr);
size_t          lastIndexOf_String          (const iString *, iChar ch);
size_t          lastIndexOfCStr_String      (const iString *, const char *cstr);

#define         indexOfString_String(d, s)      indexOfCStr_String(d, cstr_String(s))
#define         lastIndexOfString_String(d, s)  lastIndexOfCStr_String(d, cstr_String(s))

static inline iBool contains_String(const iString *d, iChar ch) {
    return indexOf_String(d, ch) != iInvalidPos;
}

void            set_String      (iString *, const iString *other);
void            setCStr_String  (iString *, const char *cstr);
void            setBlock_String (iString *, const iBlock *block);
void            format_String   (iString *, const char *format, ...);

void            append_String       (iString *, const iString *other);
void            appendCStr_String   (iString *, const char *cstr);
void            appendChar_String   (iString *, iChar ch);
void            appendRange_String  (iString *, const iRangecc *range);
void            prepend_String      (iString *, const iString *other);

void            clear_String    (iString *);
void            truncate_String (iString *, size_t len);
void            trimStart_String(iString *);
void            trimEnd_String  (iString *);
void            trim_String     (iString *);

int             toInt_String    (const iString *);

const char *    skipSpace_CStr  (const char *);

static inline iRangecc rangeN_CStr  (const char *cstr, size_t size) { return (iRangecc){ cstr, cstr + size }; }
static inline iRangecc range_CStr   (const char *cstr) { return rangeN_CStr(cstr, strlen(cstr)); }

iStringList *   split_Rangecc       (const iRangecc *, const char *separator);

/**
 * Finds the next range between separators.
 *
 * @param separator  Separator string.
 * @param range      Next range. Before the first call to the function, this must be
 *                   initialized to zero.
 *
 * @return @c iTrue, if a next range was found.
 */
iBool           nextSplit_Rangecc   (const iRangecc *, const char *separator, iRangecc *range);

const char *    findAscii_Rangecc   (const iRangecc *, char ch);

static inline iStringList *split_String(const iString *d, const char *separator) {
    const iRangecc range = range_String(d);
    return split_Rangecc(&range, separator);
}

static inline iStringList *split_CStr(const char *cstr, const char *separator) {
    return split_Rangecc(&(iRangecc){ cstr, cstr + strlen(cstr) }, separator);
}

/** @name Iterators */
///@{
iDeclareConstIterator(String, const iString *)
struct ConstIteratorImpl_String {
    iChar value;
    const char *pos;
    const iString *str;
    const char *next;
    size_t remaining;
    mbstate_t mbs;
};
///@}

//---------------------------------------------------------------------------------------

struct Impl_MultibyteChar {
    char bytes[MB_LEN_MAX + 1];
};

void init_MultibyteChar(iMultibyteChar *d, iChar ch);

struct Impl_StringComparison {
    int (*cmp)(const char *, const char *);
    int (*cmpN)(const char *, const char *, size_t);
};

#define         iCmpStr(a, b)       strcmp(a, b)
#define         iCmpStrN(a, b, len) strncmp(a, b, len)

int             iCmpStrRange(const iRangecc *range, const char *cstr);

int             iCmpStrCase (const char *a, const char *b);
int             iCmpStrNCase(const char *a, const char *b, size_t len);

extern iStringComparison iCaseSensitive;
extern iStringComparison iCaseInsensitive;
