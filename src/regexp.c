/** @file regexp.c  Perl-compatible regular expressions.

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

#include "c_plus/regexp.h"
#include "c_plus/string.h"
#include "c_plus/range.h"
#include "c_plus/object.h"

#if !defined (iHavePcre)
#   error libpcre is required for regular expressions
#endif

#include <stdio.h>
#include <pcre.h>

struct Impl_RegExp {
    iObject object;
    pcre *re;
};

iRegExp *new_RegExp(const char *pattern, enum iRegExpOption options) {
    int opts = PCRE_UTF8 | PCRE_UCP;
    if (options & caseInsensitive_RegExpOption) {
        opts |= PCRE_CASELESS;
    }
    if (options & multiLine_RegExpOption) {
        opts |= PCRE_MULTILINE;
    }
    const char *errorMsg = NULL;
    int errorOffset;
    iRegExp *d = iNew(RegExp);
    d->re = pcre_compile(pattern, opts, &errorMsg, &errorOffset, NULL);
    if (!d->re) {
        iDebug("new_RegExp: \"%s\" %s (at offset %i)\n", pattern, errorMsg, errorOffset);
    }
    return d;
}

void deinit_RegExp(iRegExp *d) {
    if (d->re) {
        pcre_free(d->re);
    }
}

iBool match_RegExp(const iRegExp *d, const char *subject, size_t len, iRegExpMatch *match) {
    if (!d->re || !subject) return iFalse;
    if (match->subject != subject) {
        // The match object is uninitialized, so initialize it now.
        iZap(*match);
        match->subject = subject;
    }
    iAssert(match->pos <= len);
    int rc = pcre_exec(d->re, NULL,
                       subject, len,
                       match->pos, 0,
                       &match->range.start,
                       iRegExpMaxSubstrings + 1);
    if (rc > 0) {
        match->pos = match->range.end;
        return iTrue;
    }
    return iFalse;
}

iString *captured_RegExpMatch(const iRegExpMatch *d, int index) {
    iAssert(index <= iRegExpMaxSubstrings);
    const iRangei *cap = &d->range + index;
    return newCStrN_String(d->subject + cap->start, size_Range(cap));
}

iDefineClass(RegExp)
