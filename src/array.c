/** @file array.c  Array of sequential fixed-size elements.

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

#include "c_plus/array.h"

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

iDefineTypeConstructionArgs(Array, (size_t elemSize), elemSize)

iArray *copy_Array(const iArray *other) {
    iArray *d = iMalloc(Array);
    d->range.start = 0;
    d->range.end = size_Array(other);
    d->allocSize = d->range.end + 1;
    d->data = malloc(d->allocSize);
    d->elementSize = other->elementSize;
    memcpy(d->data, other->data, d->range.end);
    return d;
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

void setN_Array(iArray *d, size_t pos, const void *value, size_t count) {
    iAssert(pos + count <= size_Array(d));
    memcpy(at_Array(d, pos), value, count * d->elementSize);
}

void pushBackN_Array(iArray *d, const void *value, size_t count) {
    insertN_Array(d, size_Array(d), value, count);
}

void pushFrontN_Array(iArray *d, const void *value, size_t count) {
    insertN_Array(d, 0, value, count);
}

size_t popBackN_Array(iArray *d, size_t count) {
    count = iMin(count, size_Array(d));
    removeN_Array(d, size_Array(d) - count, count);
    return count;
}

size_t popFrontN_Array(iArray *d, size_t count) {
    count = iMin(count, size_Array(d));
    removeN_Array(d, 0, count);
    return count;
}

size_t takeN_Array(iArray *d, size_t pos, void *value_out, size_t count) {
    count = iMin(count, size_Array(d) - pos);
    memcpy(value_out, at_Array(d, pos), count * d->elementSize);
    removeN_Array(d, pos, count);
    return count;
}

void insertN_Array(iArray *d, size_t pos, const void *value, size_t count) {
    if (!count) return;
    iAssert(pos <= size_Array(d));
    reserve_Array(d, size_Array(d) + count);
    rebalance_Array_(d);
    // Map to internal range.
    pos += d->range.start;
    // Easy insertions.
    if (pos == d->range.end && d->range.end + count <= d->allocSize) { // At the end.
        memcpy(element_Array_(d, pos), value, count * d->elementSize);
        d->range.end += count;
    }
    else if (d->range.start >= count && pos == d->range.start) { // At the beginning.
        pos -= count;
        memcpy(element_Array_(d, pos), value, count * d->elementSize);
        d->range.start -= count;
    }
    else {
        // Need to make some room. Shift backward?
        if (d->range.start >= count &&
                (d->range.end + count > d->allocSize ||
                 pos - d->range.start < d->range.end - pos)) {
            const iRanges moved = { d->range.start, pos };
            pos -= count;
            move_Array_(d, &moved, -(int)count);
            d->range.start -= count;
            memcpy(element_Array_(d, pos), value, count * d->elementSize);
        }
        else { // Shift forward.
            const iRanges moved = { pos, d->range.end };
            move_Array_(d, &moved, count);
            d->range.end += count;
            memcpy(element_Array_(d, pos), value, count * d->elementSize);
        }
    }
}

void removeN_Array(iArray *d, size_t pos, size_t count) {
    iAssert(pos < size_Array(d));
    iAssert(pos + count <= size_Array(d));
    pos += d->range.start;
    if (pos == d->range.end - count) {
        d->range.end -= count;
    }
    else if (pos == d->range.start) {
        d->range.start += count;
    }
    else if (pos - d->range.start > d->range.end - pos) {
        const iRanges moved = { pos + count, d->range.end };
        move_Array_(d, &moved, -(int)count);
        d->range.end -= count;
    }
    else {
        const iRanges moved = { d->range.start, pos };
        move_Array_(d, &moved, count);
        d->range.start += count;
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
