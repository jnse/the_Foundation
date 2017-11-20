#include "lite/list.h"

#include <stdlib.h>
#include <string.h>

#define LIST_MIN_ALLOC 16

#define _List_element(d, index) (&(d)->data[(d)->elementSize * (index)])

static void _List_move(List *d, const Ranges *moved, int delta) {
    char *ptr = _List_element(d, moved->start);
    memmove(ptr + delta * d->elementSize, ptr, d->elementSize * Range_size(moved));    
}

static void _List_shift(List *d, int delta) {
    LITE_ASSERT(d->range.start + delta >= 0);
    LITE_ASSERT(d->range.end   + delta <= d->allocSize);
    _List_move(d, &d->range, delta);    
    Range_shift(&d->range, delta);
}

/// Calculates the shift required to balance the list elements.
static int _List_imbalance(const List *d) {
    const int left  = d->range.start;
    const int right = d->allocSize - d->range.end;
    return (right - left) / 2;
}

static void _List_rebalance(List *d) {
    const int imbalance = _List_imbalance(d);
    if (abs(imbalance) > d->allocSize / 2) {
        _List_shift(d, imbalance);
    }
}

List *List_new(size_t elementSize) {
    List *d = calloc(sizeof(List), 1);
    d->elementSize = elementSize;
    return d;
}

void List_delete(List *d) {
    free(d);
}

void *List_data(const List *d) {
    if (List_isEmpty(d)) return NULL;    
    return _List_element(d, d->range.start);
}

void *List_at(const List *d, size_t pos) {
    LITE_ASSERT(pos < Range_size(&d->range));
    return _List_element(d, d->range.start + pos);
}

void List_reserve(List *d, size_t reservedSize) {
    size_t new_size = (d->allocSize == 0? LIST_MIN_ALLOC : d->allocSize);
    while (new_size < reservedSize) {
        new_size *= 2;
    }
    if (new_size > d->allocSize) {
        d->data = realloc(d->data, d->elementSize * new_size);
        d->allocSize = new_size;
    }
}

void List_resize(List *d, size_t size) {
    List_reserve(d, size);
    _List_rebalance(d);
}

void List_pushBack(List *d, const void *value) {
    List_insert(d, List_size(d), value);
}

void List_pushFront(List *d, const void *value) {
    List_insert(d, 0, value);
}

void List_popBack(List *d) {
    LITE_ASSERT(!List_isEmpty(d));
    return List_remove(d, List_size(d) - 1);
}

void List_popFront(List *d) {
    LITE_ASSERT(!List_isEmpty(d));
    return List_remove(d, 0);
}

void List_take(List *d, size_t pos, void *outTaken) {
    memcpy(outTaken, List_at(d, pos), d->elementSize);
    List_remove(d, pos);
}

void List_insert(List *d, size_t pos, const void *value) {
    LITE_ASSERT(pos <= List_size(d));
    List_reserve(d, List_size(d) + 1);
    pos += d->range.start;
    // Easy insertions.
    if (pos == d->range.end && d->range.end < d->allocSize) { // At the end.
        memcpy(_List_element(d, pos), value, d->elementSize);
        d->range.end++;
    }
    else if (pos == d->range.start && d->range.start > 0) { // At the beginning.
        memcpy(_List_element(d, pos), value, d->elementSize);
        d->range.start--;
    }
    else {
        // Need to make some room.
        if (pos - d->range.start < d->range.end - pos && d->range.start > 0) { // Move the start?
            const Ranges moved = { d->range.start, pos };
            _List_move(d, &moved, -1);
            d->range.start--;
            memcpy(_List_element(d, pos), value, d->elementSize);            
        }
        else {
            LITE_ASSERT(d->range.end < d->allocSize);
            const Ranges moved = { pos, d->range.end };
            _List_move(d, &moved, +1);
            d->range.end++;
            memcpy(_List_element(d, pos), value, d->elementSize);
        }
    }
    _List_rebalance(d);    
}

void List_remove(List *d, size_t pos) {
    LITE_ASSERT(pos < List_size(d));
    pos += d->range.start;
    if (pos == d->range.end - 1) {
        d->range.end--;
    }
    else if (pos == d->range.start) {
        d->range.start++;        
    }
    else if (pos - d->range.start > d->range.end - pos) {
        const Ranges moved = { pos + 1, d->range.end };
        _List_move(d, &moved, -1);
        d->range.end--;
    }
    else {
        const Ranges moved = { d->range.start, pos };
        _List_move(d, &moved, +1);
        d->range.start++;
    }
    _List_rebalance(d);
}
