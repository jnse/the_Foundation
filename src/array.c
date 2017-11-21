#include "lite/array.h"

#include <stdlib.h>
#include <string.h>

#define LIST_MIN_ALLOC 16

#define _Array_element(d, index) (&(d)->data[(d)->elementSize * (index)])

static void _Array_move(lArray *d, const lRanges *moved, int delta) {
    char *ptr = _Array_element(d, moved->start);
    memmove(ptr + delta * d->elementSize, ptr, d->elementSize * lRange_size(moved));    
}

static void _Array_shift(lArray *d, int delta) {
    LITE_ASSERT(d->range.start + delta >= 0);
    LITE_ASSERT(d->range.end   + delta <= d->allocSize);
    _Array_move(d, &d->range, delta);    
    lRange_shift(&d->range, delta);
}

/// Calculates the shift required to balance the list elements.
static int _Array_imbalance(const lArray *d) {
    const int left  = d->range.start;
    const int right = d->allocSize - d->range.end;
    return (right - left) / 2;
}

static void _Array_rebalance(lArray *d) {
    const int imbalance = _Array_imbalance(d);
    if (abs(imbalance) > d->allocSize / 2) {
        _Array_shift(d, imbalance);
    }
}

lArray *lArray_new(size_t elementSize) {
    lArray *d = calloc(sizeof(lArray), 1);
    d->elementSize = elementSize;
    return d;
}

void lArray_delete(lArray *d) {
    free(d);
}

void *lArray_data(const lArray *d) {
    if (lArray_isEmpty(d)) return NULL;    
    return _Array_element(d, d->range.start);
}

void *lArray_at(const lArray *d, size_t pos) {
    LITE_ASSERT(pos < lRange_size(&d->range));
    return _Array_element(d, d->range.start + pos);
}

void lArray_reserve(lArray *d, size_t reservedSize) {
    size_t new_size = (d->allocSize == 0? LIST_MIN_ALLOC : d->allocSize);
    while (new_size < reservedSize) {
        new_size *= 2;
    }
    if (new_size > d->allocSize) {
        d->data = realloc(d->data, d->elementSize * new_size);
        d->allocSize = new_size;
    }
}

void lArray_resize(lArray *d, size_t size) {
    lArray_reserve(d, size);
    _Array_rebalance(d);
}

void lArray_pushBack(lArray *d, const void *value) {
    lArray_insert(d, lArray_size(d), value);
}

void lArray_pushFront(lArray *d, const void *value) {
    lArray_insert(d, 0, value);
}

void lArray_popBack(lArray *d) {
    LITE_ASSERT(!lArray_isEmpty(d));
    return lArray_remove(d, lArray_size(d) - 1);
}

void lArray_popFront(lArray *d) {
    LITE_ASSERT(!lArray_isEmpty(d));
    return lArray_remove(d, 0);
}

void lArray_take(lArray *d, size_t pos, void *outTaken) {
    memcpy(outTaken, lArray_at(d, pos), d->elementSize);
    lArray_remove(d, pos);
}

void lArray_insert(lArray *d, size_t pos, const void *value) {
    LITE_ASSERT(pos <= lArray_size(d));
    lArray_reserve(d, lArray_size(d) + 1);
    pos += d->range.start;
    // Easy insertions.
    if (pos == d->range.end && d->range.end < d->allocSize) { // At the end.
        memcpy(_Array_element(d, pos), value, d->elementSize);
        d->range.end++;
    }
    else if (pos == d->range.start && d->range.start > 0) { // At the beginning.
        memcpy(_Array_element(d, pos), value, d->elementSize);
        d->range.start--;
    }
    else {
        // Need to make some room.
        if (pos - d->range.start < d->range.end - pos && d->range.start > 0) { // Move the start?
            const lRanges moved = { d->range.start, pos };
            _Array_move(d, &moved, -1);
            d->range.start--;
            memcpy(_Array_element(d, pos), value, d->elementSize);            
        }
        else {
            LITE_ASSERT(d->range.end < d->allocSize);
            const lRanges moved = { pos, d->range.end };
            _Array_move(d, &moved, +1);
            d->range.end++;
            memcpy(_Array_element(d, pos), value, d->elementSize);
        }
    }
    _Array_rebalance(d);    
}

void lArray_remove(lArray *d, size_t pos) {
    LITE_ASSERT(pos < lArray_size(d));
    pos += d->range.start;
    if (pos == d->range.end - 1) {
        d->range.end--;
    }
    else if (pos == d->range.start) {
        d->range.start++;        
    }
    else if (pos - d->range.start > d->range.end - pos) {
        const lRanges moved = { pos + 1, d->range.end };
        _Array_move(d, &moved, -1);
        d->range.end--;
    }
    else {
        const lRanges moved = { d->range.start, pos };
        _Array_move(d, &moved, +1);
        d->range.start++;
    }
    _Array_rebalance(d);
}
