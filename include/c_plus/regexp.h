#pragma once

/** @file c_plus/regexp.h  Perl-compatible regular expressions.

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
#include "range.h"

iDeclareType(String)
iDeclareType(RegExp)
iDeclareType(RegExpMatch)

#define iRegExpMaxSubstrings  32

enum iRegExpOption {
    caseSensitive_RegExpOption      = 0,
    caseInsensitive_RegExpOption    = 0x1,
    multiLine_RegExpOption          = 0x2,
};

iRegExp *   new_RegExp(const char *pattern, enum iRegExpOption options);
void        delete_RegExp(iRegExp *);

#define     collect_RegExp(d)   iCollectDel(d, delete_RegExp)

iBool       match_RegExp(const iRegExp *, const char *subject, size_t len, iRegExpMatch *match);

#define     matchString_RegExp(d, str, m)   match_RegExp(d, cstr_String(str), size_String(str), m)

struct Impl_RegExpMatch {
    const char *subject;
    size_t pos;
    iRangei range;
    iRangei substring[iRegExpMaxSubstrings];
    int data_[iRegExpMaxSubstrings + 1];
};

iString *   captured_RegExpMatch(const iRegExpMatch *, int index);
