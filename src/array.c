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

#define iArrayMinAlloc 4

#define element_Array_(d, index) (&(d)->data[(d)->elementSize * (index)])

static void move_Array_(iArray *d, const iRanges *moved, int delta) {
    if (delta != 0) {
        char *ptr = element_Array_(d, moved->start);
        memmove(ptr + delta * d->elementSize, ptr, d->elementSize * size_Range(moved));
    }
}

static void shift_Array_(iArray *d, int delta) {
    if (delta != 0) {
        iAssert(d->range.start + delta >= 0);
        iAssert(d->range.end   + delta <= d->allocSize);
        move_Array_(d, &d->range, delta);
        shift_Range(&d->range, delta);
    }
}

/// Calculates the shift required to balance the list elements.
static int imbalance_Array_(const iArray *d) {
    const int left  = d->range.start;
    const int right = d->allocSize - d->range.end;
    return (right - left) / 2;
}

static void rebalance_Array_(iArray *d) {
    const int imbalance = imbalance_Array_(d);
    if (d->range.end == d->allocSize || d->range.start == 0) {
        shift_Array_(d, imbalance);
    }
}

iArray *new_Array(size_t elementSize) {
    iArray *d = calloc(sizeof(iArray), 1);
    d->elementSize = elementSize;
    return d;
}

void delete_Array(iArray *d) {
    deinit_Array(d);
    free(d);
}

void init_Array(iArray *d, size_t elementSize) {
    iZap(*d);
    d->elementSize = elementSize;
}

void deinit_Array(iArray *d) {
    free(d->data);
}

void *data_Array(const iArray *d) {
    if (isEmpty_Array(d)) return NULL;
    return element_Array_(d, d->range.start);
}

void *at_Array(const iArray *d, size_t pos) {
    iAssert(pos < size_Range(&d->range));
    return element_Array_(d, d->range.start + pos);
}

const void *constEnd_Array(const iArray *d) {
    return element_Array_(d, d->range.end);
}

void reserve_Array(iArray *d, size_t reservedSize) {
    size_t newSize = (d->allocSize == 0? iArrayMinAlloc : d->allocSize);
    while (newSize < reservedSize) {
        newSize *= 2;
    }
    if (newSize > d->allocSize) {
        d->data = realloc(d->data, d->elementSize * newSize);
        d->allocSize = newSize;
    }
}

void clear_Array(iArray *d) {
    d->range.start = d->range.end = d->allocSize / 2;
}

void resize_Array(iArray *d, size_t size) {
    const size_t oldSize = size_Range(&d->range);
    if (size == oldSize) return;
    if (size < oldSize) {
        setSize_Range(&d->range, size);
        return;
    }
    reserve_Array(d, size);
    if (d->range.start + size > d->allocSize) {
        // Rebalance according to the new size.
        const int leftSide = (d->allocSize - size) / 2;
        shift_Array_(d, leftSide - (int) d->range.start);
    }
    setSize_Range(&d->range, size);
    // Zero newly added elements.
    memset(element_Array_(d, d->range.start + oldSize), 0, d->elementSize * (size - oldSize));
}

void set_Array(iArray *d, size_t pos, const void *value) {
    memcpy(at_Array(d, pos), value, d->elementSize);
}

void pushBack_Array(iArray *d, const void *value) {
    insert_Array(d, size_Array(d), value);
}

void pushFront_Array(iArray *d, const void *value) {
    insert_Array(d, 0, value);
}

iBool popBack_Array(iArray *d)
{
    if (isEmpty_Array(d)) return iFalse;
    remove_Array(d, size_Array(d) - 1);
    return iTrue;
}

iBool popFront_Array(iArray *d) {
    if (isEmpty_Array(d)) return iFalse;
    remove_Array(d, 0);
    return iTrue;
}

iBool take_Array(iArray *d, size_t pos, void *outValue) {
    if (pos < size_Array(d)) {
        memcpy(outValue, at_Array(d, pos), d->elementSize);
        remove_Array(d, pos);
        return iTrue;
    }
    return iFalse;
}

void insert_Array(iArray *d, size_t pos, const void *value) {
    iAssert(pos <= size_Array(d));
    reserve_Array(d, size_Array(d) + 1);
    rebalance_Array_(d);
    pos += d->range.start;
    // Easy insertions.
    if (pos == d->range.end && d->range.end < d->allocSize) { // At the end.
        memcpy(element_Array_(d, pos), value, d->elementSize);
        d->range.end++;
    }
    else if (pos == d->range.start && d->range.start > 0) { // At the beginning.
        memcpy(element_Array_(d, --pos), value, d->elementSize);
        d->range.start--;
    }
    else {
        // Need to make some room. Shift backward?
        if (d->range.end == d->allocSize || pos - d->range.start < d->range.end - pos) {
            iAssert(d->range.start > 0);
            const iRanges moved = { d->range.start, pos-- };
            move_Array_(d, &moved, -1);
            d->range.start--;
            memcpy(element_Array_(d, pos), value, d->elementSize);
        }
        else { // Shift forward.
            const iRanges moved = { pos, d->range.end };
            move_Array_(d, &moved, +1);
            d->range.end++;
            memcpy(element_Array_(d, pos), value, d->elementSize);
        }
    }
}

void remove_Array(iArray *d, size_t pos) {
    iAssert(pos < size_Array(d));
    pos += d->range.start;
    if (pos == d->range.end - 1) {
        d->range.end--;
    }
    else if (pos == d->range.start) {
        d->range.start++;
    }
    else if (pos - d->range.start > d->range.end - pos) {
        const iRanges moved = { pos + 1, d->range.end };
        move_Array_(d, &moved, -1);
        d->range.end--;
    }
    else {
        const iRanges moved = { d->range.start, pos };
        move_Array_(d, &moved, +1);
        d->range.start++;
    }
}

void fill_Array(iArray *d, char value) {
    memset(front_Array(d), value, d->elementSize * size_Array(d));
}

void sort_Array(iArray *d, int (*cmp)(const void *, const void *)) {
    qsort(front_Array(d), size_Array(d), d->elementSize, cmp);
}

//---------------------------------------------------------------------------------------

void init_ArrayIterator(iArrayIterator *d, iArray *array) {
    d->array = array;
    d->pos = 0;
    d->value = (!isEmpty_Array(array)? at_Array(array, 0) : NULL);
}

void next_ArrayIterator(iArrayIterator *d) {
    if (d->pos < size_Array(d->array)) {
        d->value = at_Array(d->array, ++d->pos);
    }
    else {
        d->value = NULL;
    }
}

void init_ArrayConstIterator(iArrayConstIterator *d, const iArray *array) {
    d->array = array;
    d->value = (!isEmpty_Array(array)? at_Array(array, 0) : NULL);
    d->end = element_Array_(d->array, d->array->range.end);
}

void next_ArrayConstIterator(iArrayConstIterator *d) {
    iAssert(d->value);
    d->value = (const char *) d->value + d->array->elementSize;
    if (d->value >= d->end) {
        d->value = NULL;
    }
}
