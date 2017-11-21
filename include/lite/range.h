/** @file lite/range.h  Numeric ranges.

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

#pragma once

#include "defs.h"

#define iRange_size(self)               ((self)->end - (self)->start)
#define iRange_isEmpty(self)            ((self)->end == (self)->start)
#define iRange_contains(self, value)    ((value) >= (self)->start && (value) < (self)->end)
#define iRange_shift(self, delta)       {(self)->start += (delta); (self)->end += (delta);}

struct i_Rangei_Impl {
    int start;
    int end;
};

struct i_Rangeui_Impl {
    unsigned int start;
    unsigned int end;
};

struct i_Ranges_Impl {
    size_t start;
    size_t end;
};

LITE_DECLARE_IMPL(Rangei);
LITE_DECLARE_IMPL(Rangeui);
LITE_DECLARE_IMPL(Ranges);
