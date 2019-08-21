#pragma once

/** @file the_Foundation/string.h  UTF-8 string with copy-on-write semantics.

String is derived from Block, and contains text with multibyte characters. When
iterating a string, the multibyte characters are converted to UTF-32 code points.

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

iBeginPublic

typedef uint32_t iChar;

iChar   upper_Char  (iChar);
iChar   lower_Char  (iChar);
iBool   isSpace_Char(iChar);

iDeclareType(String)
iDeclareType(StringList)
iDeclareType(StringComparison)
iDeclareType(MultibyteChar)

iDeclareType(Stream)

struct Impl_String {
    iBlock chars;
};

#define iStringLiteral(str)     (iString){ iBlockLiteral(str, strlen(str), strlen(str) + 1) }

iDeclareTypeConstruction(String)

iString *       newCStr_String      (const char *utf8CStr);
iString *       newCStrN_String     (const char *utf8CStr, size_t n);
iString *       newUtf16_String     (const uint16_t *utf16Str);
iString *       newUtf16N_String    (const uint16_t *utf16Str, size_t n);
iString *       newUnicode_String   (const iChar *ucs);
iString *       newUnicodeN_String  (const iChar *ucs, size_t n);
iString *       newLocalCStr_String (const char *localCStr);
iString *       newLocalCStrN_String(const char *localCStr, size_t n);
iString *       newBlock_String     (const iBlock *utf8Data);
iString *       newFormat_String    (const char *format, ...);
iString *       copy_String         (const iString *);

iString *       collectNewFormat_String (const char *format, ...);

iLocalDef iString *newRange_String  (const iRangecc *range) { return newCStrN_String(range->start, size_Range(range)); }
iLocalDef iString *newLocal_String  (const iBlock *localChars) { return newLocalCStrN_String(cstr_Block(localChars), size_Block(localChars)); }

void            init_String             (iString *);
void            initCStr_String         (iString *, const char *utf8CStr);
void            initCStrN_String        (iString *, const char *utf8CStr, size_t n);
void            initUtf16_String        (iString *, const uint16_t *utf16Str);
void            initUtf16N_String       (iString *, const uint16_t *utf16Str, size_t n);
void            initUnicode_String      (iString *, const iChar *ucs);
void            initUnicodeN_String     (iString *, const iChar *ucs, size_t n);
void            initLocalCStr_String    (iString *, const char *localCStr);
void            initLocalCStrN_String   (iString *, const char *localCStr, size_t n);
void            initBlock_String        (iString *, const iBlock *chars);
void            initCopy_String         (iString *, const iString *other);

const char *    cstr_String     (const iString *);
size_t          length_String   (const iString *);
size_t          size_String     (const iString *);
iString *       mid_String      (const iString *, size_t charStartPos, size_t charCount);
iString *       upper_String    (const iString *);
iString *       lower_String    (const iString *);
iStringList *   split_String    (const iString *, const char *separator);
iString *       urlEncode_String(const iString *);
iString *       urlDecode_String(const iString *);
iChar           first_String    (const iString *);
iBlock *        toLocal_String  (const iString *);
iBlock *        toUtf16_String  (const iString *);

/**
 * Returns a pointer to the string converted to the current locale's encoding.
 * The temporary conversion is collected as garbage.
 *
 * @param str  String to convert.
 *
 * @return Converted text. The pointer will remain valid until garbage is recycled.
 */
iLocalDef const char *cstrLocal_String(const iString *str) {
    return cstr_Block(collect_Block(toLocal_String(str)));
}

#define         range_String(d) (iRangecc){ constBegin_Block(&d->chars), constEnd_Block(&d->chars) }

iLocalDef iBool         isEmpty_String   (const iString *d) { return size_String(d) == 0; }
iLocalDef const char *  constBegin_String(const iString *d) { return cstr_String(d); }
iLocalDef const char *  constEnd_String  (const iString *d) { return cstr_String(d) + size_String(d); }

int             cmpSc_String        (const iString *, const char *cstr, const iStringComparison *);
int             cmpNSc_String       (const iString *, const char *cstr, size_t n, const iStringComparison *);

#define         cmp_String(d, cstr)             cmpSc_String(d, cstr, &iCaseSensitive)
#define         cmpCase_String(d, cstr)         cmpSc_String(d, cstr, &iCaseInsensitive)
#define         cmpString_String(d, s)          cmpSc_String(d, cstr_String(s), &iCaseSensitive)
#define         cmpStringCase_String(d, s)      cmpSc_String(d, cstr_String(s), &iCaseInsensitive)
#define         cmpStringSc_String(d, s, sc)    cmpSc_String(d, cstr_String(s), sc)

iBool           startsWithSc_String (const iString *, const char *cstr, const iStringComparison *);
iBool           endsWithSc_String   (const iString *, const char *cstr, const iStringComparison *);

#define         startsWith_String(d, cstr)      startsWithSc_String(d, cstr, &iCaseSensitive)
#define         startsWithCase_String(d, cstr)  startsWithSc_String(d, cstr, &iCaseInsensitive)
#define         endsWith_String(d, cstr)        endsWithSc_String  (d, cstr, &iCaseSensitive)
#define         endsWithCase_String(d, cstr)    endsWithSc_String  (d, cstr, &iCaseInsensitive)

size_t          indexOf_String              (const iString *, iChar ch);
size_t          indexOfCStr_String          (const iString *, const char *cstr);
size_t          indexOfCStrFrom_String      (const iString *, const char *cstr, size_t from);
size_t          indexOfCStrSc_String        (const iString *, const char *cstr, const iStringComparison *);
size_t          indexOfCStrFromSc_String    (const iString *, const char *cstr, size_t from, const iStringComparison *);
size_t          lastIndexOf_String          (const iString *, iChar ch);
size_t          lastIndexOfCStr_String      (const iString *, const char *cstr);

#define         indexOfString_String(d, s)      indexOfCStr_String(d, cstr_String(s))
#define         lastIndexOfString_String(d, s)  lastIndexOfCStr_String(d, cstr_String(s))

iLocalDef iBool contains_String(const iString *d, iChar ch) {
    return indexOf_String(d, ch) != iInvalidPos;
}

void            set_String      (iString *, const iString *other);
void            setCStr_String  (iString *, const char *cstr);
void            setBlock_String (iString *, const iBlock *block);
void            format_String   (iString *, const char *format, ...);

void            serialize_String    (const iString *, iStream *outs);
void            deserialize_String  (iString *, iStream *ins);

void            append_String       (iString *, const iString *other);
void            appendCStr_String   (iString *, const char *cstr);
void            appendCStrN_String  (iString *, const char *cstr, size_t size);
void            appendChar_String   (iString *, iChar ch);
void            appendRange_String  (iString *, const iRangecc *range);
void            appendFormat_String (iString *, const char *format, ...);
void            prepend_String      (iString *, const iString *other);
void            prependChar_String  (iString *, iChar ch);
void            prependCStr_String  (iString *, const char *cstr);

void            clear_String    (iString *);
void            truncate_String (iString *, size_t len);
void            trimStart_String(iString *);
void            trimEnd_String  (iString *);
void            trim_String     (iString *);

int             toInt_String    (const iString *);
float           toFloat_String  (const iString *);
double          toDouble_String (const iString *);

const char *    skipSpace_CStr  (const char *);

iLocalDef iRangecc rangeN_CStr  (const char *cstr, size_t size) {
#if __STDC_VERSION__ >= 201100L
    return (iRangecc){ cstr, cstr + size };
#else
    const iRangecc range = { cstr, cstr + size };
    return range;
#endif
}
iLocalDef iRangecc range_CStr   (const char *cstr) { return rangeN_CStr(cstr, strlen(cstr)); }

#define         cmp_Rangecc(d, cstr) cmpCStrSc_Rangecc((d), (cstr), &iCaseSensitive)
int             cmpCStrSc_Rangecc   (const iRangecc *, const char *cstr, const iStringComparison *);
int             cmpCStrNSc_Rangecc  (const iRangecc *, const char *cstr, size_t n, const iStringComparison *);
iBool           startsWithSc_Rangecc(const iRangecc *, const char *cstr, const iStringComparison *);
iStringList *   split_Rangecc       (const iRangecc *, const char *separator);
void            trimStart_Rangecc   (iRangecc *);
void            trimEnd_Rangecc     (iRangecc *);
void            trim_Rangecc        (iRangecc *);

/**
 * Finds the next range between separators. Empty ranges at the beginning and end of
 * the string are ignored (i.e., when there is a separator at the beginning or the end
 * of the string).
 *
 * A string containing nothing but the separator results in no split ranges.
 *
 * @param separator  Separator string.
 * @param range      Next range. Must be initialized to zero. Subsequent ranges are
 *                   searched based on the locations pointed to by this variable.
 *
 * @return @c iTrue, if a next range was found (@a range was updated).
 */
iBool           nextSplit_Rangecc   (const iRangecc *, const char *separator, iRangecc *range);

const char *    findAscii_Rangecc   (const iRangecc *, char ch);

iStringList *   split_CStr  (const char *cstr, const char *separator);

/** @name Iterators */
///@{
iDeclareConstIterator(String, const iString *)
struct ConstIteratorImpl_String {
    iChar value;
    const char *pos;
    const char *next;
    const iString *str;
    size_t remaining;
};
///@}

/*-------------------------------------------------------------------------------------*/

#define iMultibyteCharMaxSize ((size_t) 7)

struct Impl_MultibyteChar {
    char bytes[8]; // UTF-8 encoding
};

void    init_MultibyteChar          (iMultibyteChar *d, iChar ch);
int     decodeBytes_MultibyteChar   (const char *bytes, size_t n, iChar *ch_out);

const char *    cstrLocal_Char  (iChar ch); // locale-encoding

int             iCmpStr     (const char *a, const char *b);
int             iCmpStrN    (const char *a, const char *b, size_t n);
int             iCmpStrRange(const iRangecc *range, const char *cstr);
int             iCmpStrCase (const char *a, const char *b);
int             iCmpStrNCase(const char *a, const char *b, size_t len);

struct Impl_StringComparison {
    int     (*cmp)      (const char *, const char *);
    int     (*cmpN)     (const char *, const char *, size_t);
    char *  (*locate)   (const char *, const char *);
};

iFoundationAPIData iStringComparison iCaseSensitive;
iFoundationAPIData iStringComparison iCaseInsensitive;

char *          iDupStr     (const char *);
char *          iStrStrN    (const char *, const char *, size_t);

iEndPublic
