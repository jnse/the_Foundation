#pragma once

#include "defs.h"
#include "range.h"

struct i_Array_Impl {
    iRanges range;
    size_t allocSize;
    size_t elementSize;
    char *data;
};

LITE_DECLARE_IMPL(Array);

iArray *    iArray_new(size_t elementSize);
void        iArray_delete(iArray *);

#define     iArray_isEmpty(d)   iRange_isEmpty(&(d)->range)
#define     iArray_size(d)      iRange_size(&(d)->range)
#define     iArray_front(d)     iArray_at(d, 0)
#define     iArray_back(d)      iArray_at(d, iArray_size(d) - 1)

void *      iArray_data(const iArray *);
void *      iArray_at(const iArray *, size_t pos);

void        iArray_reserve(iArray *, size_t reservedSize);
void        iArray_resize(iArray *, size_t size);
void        iArray_pushBack(iArray *, const void *value);
void        iArray_pushFront(iArray *, const void *value);
void        iArray_popBack(iArray *);
void        iArray_popFront(iArray *);
void        iArray_take(iArray *, size_t pos, void *outTaken);
void        iArray_insert(iArray *, size_t pos, const void *value);
void        iArray_remove(iArray *, size_t pos);
