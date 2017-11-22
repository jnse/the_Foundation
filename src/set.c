/** @file set.c  Set of unique integer values.

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

#include "lite/set.h"

#include <stdlib.h>

iSet *new_Set(void) {
    iSet *d = calloc(sizeof(iSet), 1);
    init_Set(d);
    return d;
}

void delete_Set(iSet *d) {
    deinit_Set(d);
    free(d);
}

void init_Set(iSet *d) {
    init_Array(&d->values, sizeof(iSetValue));
}

void deinit_Set(iSet *d) {
    deinit_Array(&d->values);
}

size_t size_Set(const iSet *d) {
    return size_Array(&d->values);
}

iBool contains_Set(const iSet *d, iSetValue value) {
    return locate_Set(d, value, NULL);
}

iBool locate_Set(const iSet *d, iSetValue value, iRanges *span) {
    iRanges loc;
    if (!span) span = &loc;

    // We will narrow down the span until the pointer is found or we'll know where
    // it would be if it were inserted.
    span->start = 0;
    span->end = size_Array(&d->values);

    while (!isEmpty_Range(span)) {
        // Arrived at a single item?
        if (size_Range(span) == 1) {
            if (at_Set(d, span->start) == value) {
                return iTrue; // Found it.
            }
            // Then the value would go before or after this position.
            if (value < at_Set(d, span->start)) {
                span->end = span->start;
            }
            else {
                span->start = span->end;
            }
            return iFalse;
        }
        // Narrow down the search by a half.
        const size_t rightHalf = (span->start + span->end + 1) / 2;
        const iSetValue mid = at_Set(d, rightHalf);
        if (value == mid) {
            // Oh, it's here.
            span->start = rightHalf;
            span->end   = rightHalf + 1;
            return iTrue;
        }
        else if (value > mid) {
            span->start = rightHalf;
        }
        else {
            span->end = rightHalf;
        }
    }
    return iFalse;
}

iSetValue at_Set(const iSet *d, size_t pos) {
    iSetValue value;
    memcpy(&value, at_Array(&d->values, pos), sizeof(value));
    return value;
}

void clear_Set(iSet *d) {
    clear_Array(&d->values);
}

iBool insert_Set(iSet *d, iSetValue value) {
    iRanges loc;
    if (locate_Set(d, value, &loc)) {
        // The value already exists in the set.
        return iFalse;
    }
    insert_Array(&d->values, loc.start, &value);
    return iTrue;
}

iBool remove_Set(iSet *d, iSetValue value) {
    iRanges loc;
    if (locate_Set(d, value, &loc)) {
        remove_Array(&d->values, loc.start);
        return iTrue;
    }
    return iFalse;
}
