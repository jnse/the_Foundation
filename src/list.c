#include "lite/list.h"

#include <stdlib.h>
#include <string.h>

#define LIST_MIN_ALLOC 16
#define SELF List *d

static void _List_Shift(SELF, int delta) {
    LITE_ASSERT(d->range.start + delta >= 0);
    LITE_ASSERT(d->range.end   + delta <= d->alloc_size);
    char *front = List_Front(d);
    memmove(front + d->element_size * delta, front, d->element_size * List_Size(d));
    Range_Shift(&d->range, delta);
}

/// Calculates the shift required to balance the list elements.
static int _List_Imbalance(const SELF) {
    const int left  = d->range.start;
    const int right = d->alloc_size - d->range.end;
    return (right - left) / 2;
}

static void _List_Rebalance(SELF) {
    const int imbalance = _List_Imbalance(d);
    if (abs(imbalance) > d->alloc_size / 2) {
        _List_Shift(d, imbalance);
    }
}

List *List_New(size_t element_size) {
    List *d = calloc(sizeof(List), 1);
    d->element_size = element_size;
    return d;
}

void List_Delete(SELF) {
    free(d);
}

void *List_Data(const SELF) {
    if (List_IsEmpty(d)) return NULL;    
    return &d->data[d->element_size * d->range.start];
}

void *List_At(const SELF, size_t pos) {
    LITE_ASSERT(pos < Range_Size(&d->range));
    return &d->data[d->element_size * (d->range.start + pos)];
}

void List_Reserve(SELF, size_t reserved_size) {
    size_t new_size = (d->alloc_size == 0? LIST_MIN_ALLOC : d->alloc_size);
    while (new_size < reserved_size) {
        new_size *= 2;
    }
    if (new_size > d->alloc_size) {
        d->data = realloc(d->data, d->element_size * new_size);
        d->alloc_size = new_size;
    }
}

void List_Resize(SELF, size_t size) {
    List_Reserve(d, size);
    _List_Rebalance(d);
}

void List_PushBack(SELF, const void *value) {
    List_Insert(d, List_Size(d), value);
}

void List_PushFront(SELF, const void *value) {
    List_Insert(d, 0, value);
}

void List_PopBack(SELF) {
    LITE_ASSERT(!List_IsEmpty(d));
    return List_Remove(d, List_Size(d) - 1);
}

void List_PopFront(SELF) {
    LITE_ASSERT(!List_IsEmpty(d));
    return List_Remove(d, 0);
}

void List_Take(SELF, size_t pos, void *out_taken) {
    memcpy(out_taken, List_At(d, pos), d->element_size);
    List_Remove(d, pos);
}

void List_Insert(SELF, size_t pos, const void *value) {
    LITE_ASSERT(pos <= List_Size(d));
    
    _List_Rebalance(d);    
}

void List_Remove(SELF, size_t pos) {
    LITE_ASSERT(pos < List_Size(d));
    if (pos == d->range.end - 1) {
        d->range.end--;
    }
    else if (pos == d->range.start) {
        d->range.start++;        
    }
    else if (pos - d->range.start > d->range.end - pos) {
        char *ptr = List_At(d, pos);
        memmove(ptr, ptr + d->element_size, d->element_size * (d->range.end - pos - 1));
        d->range.end--;
    }
    else if (pos > d->range.start) {
        char *ptr = List_Data(d);
        memmove(ptr + d->element_size, ptr, d->element_size * (pos - d->range.start - 1));
        d->range.start++;
    }
    _List_Rebalance(d);
}
