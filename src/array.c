/** @file array.c  Array of sequential fixed-size elements.

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

#include "lite/array.h"

#include <stdlib.h>
#include <string.h>

#define LITE_ARRAY_MIN_ALLOC 16

#define i_Array_element(d, index) (&(d)->data[(d)->elementSize * (index)])

static void i_Array_move(iArray *d, const iRanges *moved, int delta) {
    if (delta != 0) {
        char *ptr = i_Array_element(d, moved->start);
        memmove(ptr + delta * d->elementSize, ptr, d->elementSize * iRange_size(moved));
    }
}

static void i_Array_shift(iArray *d, int delta) {
    if (delta != 0) {
        LITE_ASSERT(d->range.start + delta >= 0);
        LITE_ASSERT(d->range.end   + delta <= d->allocSize);
        i_Array_move(d, &d->range, delta);
        iRange_shift(&d->range, delta);
    }
}

/// Calculates the shift required to balance the list elements.
static int i_Array_imbalance(const iArray *d) {
    const int left  = d->range.start;
    const int right = d->allocSize - d->range.end;
    return (right - left) / 2;
}

static void i_Array_rebalance(iArray *d) {
    const int imbalance = i_Array_imbalance(d);
    if (d->range.end == d->allocSize || d->range.start == 0) {
        i_Array_shift(d, imbalance);
    }
}

iArray *iArray_new(size_t elementSize) {
    iArray *d = calloc(sizeof(iArray), 1);
    d->elementSize = elementSize;
    return d;
}

void iArray_delete(iArray *d) {
    free(d);
}

void *iArray_data(const iArray *d) {
    if (iArray_isEmpty(d)) return NULL;
    return i_Array_element(d, d->range.start);
}

void *iArray_at(const iArray *d, size_t pos) {
    LITE_ASSERT(pos < iRange_size(&d->range));
    return i_Array_element(d, d->range.start + pos);
}

void iArray_reserve(iArray *d, size_t reservedSize) {
    size_t newSize = (d->allocSize == 0? LITE_ARRAY_MIN_ALLOC : d->allocSize);
    while (newSize < reservedSize) {
        newSize *= 2;
    }
    if (newSize > d->allocSize) {
        d->data = realloc(d->data, d->elementSize * newSize);
        d->allocSize = newSize;
    }
}

void iArray_resize(iArray *d, size_t size) {
    iArray_reserve(d, size);
    i_Array_rebalance(d);
}

void iArray_pushBack(iArray *d, const void *value) {
    iArray_insert(d, iArray_size(d), value);
}

void iArray_pushFront(iArray *d, const void *value) {
    iArray_insert(d, 0, value);
}

iBool iArray_popBack(iArray *d) {
    if (iArray_isEmpty(d)) return iFalse;
    iArray_remove(d, iArray_size(d) - 1);
    return iTrue;
}

iBool iArray_popFront(iArray *d) {
    if (iArray_isEmpty(d)) return iFalse;
    iArray_remove(d, 0);
    return iTrue;
}

iBool iArray_take(iArray *d, size_t pos, void *outTaken) {
    if (pos < iArray_size(d)) {
        memcpy(outTaken, iArray_at(d, pos), d->elementSize);
        iArray_remove(d, pos);
        return iTrue;
    }
    return iFalse;
}

void iArray_insert(iArray *d, size_t pos, const void *value) {
    LITE_ASSERT(pos <= iArray_size(d));
    iArray_reserve(d, iArray_size(d) + 1);
    i_Array_rebalance(d);
    pos += d->range.start;
    // Easy insertions.
    if (pos == d->range.end && d->range.end < d->allocSize) { // At the end.
        memcpy(i_Array_element(d, pos), value, d->elementSize);
        d->range.end++;
    }
    else if (pos == d->range.start && d->range.start > 0) { // At the beginning.
        memcpy(i_Array_element(d, --pos), value, d->elementSize);
        d->range.start--;
    }
    else {
        // Need to make some room. Shift backward?
        if (d->range.end == d->allocSize || pos - d->range.start < d->range.end - pos) {
            LITE_ASSERT(d->range.start > 0);
            const iRanges moved = { d->range.start, pos-- };
            i_Array_move(d, &moved, -1);
            d->range.start--;
            memcpy(i_Array_element(d, pos), value, d->elementSize);
        }
        else { // Shift forward.
            const iRanges moved = { pos, d->range.end };
            i_Array_move(d, &moved, +1);
            d->range.end++;
            memcpy(i_Array_element(d, pos), value, d->elementSize);
        }
    }
}

void iArray_remove(iArray *d, size_t pos) {
    LITE_ASSERT(pos < iArray_size(d));
    pos += d->range.start;
    if (pos == d->range.end - 1) {
        d->range.end--;
    }
    else if (pos == d->range.start) {
        d->range.start++;
    }
    else if (pos - d->range.start > d->range.end - pos) {
        const iRanges moved = { pos + 1, d->range.end };
        i_Array_move(d, &moved, -1);
        d->range.end--;
    }
    else {
        const iRanges moved = { d->range.start, pos };
        i_Array_move(d, &moved, +1);
        d->range.start++;
    }
}
