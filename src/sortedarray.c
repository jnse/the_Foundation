/** @file sortedarray.c  SortedArray of unique integer values.

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

#include "c_plus/sortedarray.h"

#include <stdlib.h>

iDefineTypeConstructionArgs(SortedArray,
                            (size_t elementSize, iSortedArrayCompareElemFunc cmp),
                            elementSize, cmp)

void init_SortedArray(iSortedArray *d, size_t elementSize, iSortedArrayCompareElemFunc cmp) {
    init_Array(&d->values, elementSize);
    d->cmp = cmp;
}

void deinit_SortedArray(iSortedArray *d) {
    deinit_Array(&d->values);
}

size_t size_SortedArray(const iSortedArray *d) {
    return size_Array(&d->values);
}

iBool contains_SortedArray(const iSortedArray *d, const void *value) {
    return locate_SortedArray(d, value, NULL);
}

iBool locate_SortedArray(const iSortedArray *d, const void *value, iRanges *span) {
    iRanges loc;
    if (!span) span = &loc;
    // We will narrow down the span until the pointer is found or we'll know where
    // it would be if it were inserted.
    span->start = 0;
    span->end = size_Array(&d->values);
    while (!isEmpty_Range(span)) {
        // Arrived at a single item?
        if (size_Range(span) == 1) {
            if (d->cmp(value, at_SortedArray(d, span->start)) == 0) {
                return iTrue; // Found it.
            }
            // Then the value would go before or after this position.
            if (d->cmp(value, at_SortedArray(d, span->start)) < 0) {
                span->end = span->start;
            }
            else {
                span->start = span->end;
            }
            return iFalse;
        }
        // Narrow down the search by a half.
        const size_t rightHalf = (span->start + span->end + 1) / 2;
        const void *mid = at_SortedArray(d, rightHalf);
        if (d->cmp(value, mid) == 0) {
            // Oh, it's here.
            span->start = rightHalf;
            span->end   = rightHalf + 1;
            return iTrue;
        }
        else if (d->cmp(value, mid) > 0) {
            span->start = rightHalf;
        }
        else {
            span->end = rightHalf;
        }
    }
    return iFalse;
}

void clear_SortedArray(iSortedArray *d) {
    clear_Array(&d->values);
}

iBool insert_SortedArray(iSortedArray *d, const void *value) {
    iRanges loc;
    if (locate_SortedArray(d, value, &loc)) {
        // The value already exists in the set. It is written anyway, since some
        // contents of the element may have changed.
        set_Array(&d->values, loc.start, value);
        return iFalse; // No new elements.
    }
    insert_Array(&d->values, loc.start, value);
    return iTrue;
}

iBool remove_SortedArray(iSortedArray *d, const void *value) {
    iRanges loc;
    if (locate_SortedArray(d, value, &loc)) {
        remove_Array(&d->values, loc.start);
        return iTrue;
    }
    return iFalse;
}
