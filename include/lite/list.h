#pragma once

#include "defs.h"
#include "range.h" 

struct List_Impl {
    Ranges range;
    size_t alloc_size;
    size_t element_size;
    char *data;
};

LITE_DECLARE_OPAQUE(List);
#define SELF List *

List *      List_New(size_t element_size);
void        List_Delete(SELF);

#define     List_IsEmpty(SELF)  Range_IsEmpty(&(SELF)->range)
#define     List_Size(SELF)     Range_Size(&(SELF)->range)
#define     List_Front(SELF)    List_At(SELF, 0)
#define     List_Back(SELF)     List_At(SELF, List_Size(SELF) - 1)

void *      List_Data(const SELF);
void *      List_At(const SELF, size_t pos);

void        List_Reserve(SELF, size_t reserved_size);
void        List_Resize(SELF, size_t size);
void        List_PushBack(SELF, const void *value);
void        List_PushFront(SELF, const void *value);
void        List_PopBack(SELF);
void        List_PopFront(SELF);
void        List_Take(SELF, size_t pos, void *out_taken);
void        List_Insert(SELF, size_t pos, const void *value);
void        List_Remove(SELF, size_t pos);

#undef SELF

