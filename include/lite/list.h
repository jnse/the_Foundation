#pragma once

#include "defs.h"
#include "range.h" 

struct List_Impl {
    Ranges range;
    size_t allocSize;
    size_t elementSize;
    char *data;
};

LITE_DECLARE_OPAQUE(List);

List *      List_new(size_t elementSize);
void        List_delete(List *);

#define     List_isEmpty(d)  Range_isEmpty(&(d)->range)
#define     List_size(d)     Range_size(&(d)->range)
#define     List_front(d)    List_at(d, 0)
#define     List_back(d)     List_at(d, List_size(d) - 1)

void *      List_data(const List *);
void *      List_at(const List *, size_t pos);

void        List_reserve(List *, size_t reservedSize);
void        List_resize(List *, size_t size);
void        List_pushBack(List *, const void *value);
void        List_pushFront(List *, const void *value);
void        List_popBack(List *);
void        List_popFront(List *);
void        List_take(List *, size_t pos, void *outTaken);
void        List_insert(List *, size_t pos, const void *value);
void        List_remove(List *, size_t pos);

