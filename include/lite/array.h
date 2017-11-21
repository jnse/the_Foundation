#pragma once

#include "defs.h"
#include "range.h" 

struct lArray_Impl {
    lRanges range;
    size_t allocSize;
    size_t elementSize;
    char *data;
};

LITE_DECLARE_IMPL(Array);

lArray *    lArray_new(size_t elementSize);
void        lArray_delete(lArray *);

#define     lArray_isEmpty(d)   lRange_isEmpty(&(d)->range)
#define     lArray_size(d)      lRange_size(&(d)->range)
#define     lArray_front(d)     lArray_at(d, 0)
#define     lArray_back(d)      lArray_at(d, lArray_size(d) - 1)

void *      lArray_data(const lArray *);
void *      lArray_at(const lArray *, size_t pos);

void        lArray_reserve(lArray *, size_t reservedSize);
void        lArray_resize(lArray *, size_t size);
void        lArray_pushBack(lArray *, const void *value);
void        lArray_pushFront(lArray *, const void *value);
void        lArray_popBack(lArray *);
void        lArray_popFront(lArray *);
void        lArray_take(lArray *, size_t pos, void *outTaken);
void        lArray_insert(lArray *, size_t pos, const void *value);
void        lArray_remove(lArray *, size_t pos);

