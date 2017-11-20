#include "lite/object.h"

#include <stdio.h>
#include <stdlib.h>

struct Object_Impl {
    uint ref_count;
    Object *parent;
};

static void _Object_Delete(Object *this) {
    LITE_ASSERT(this->parent == NULL);
    printf("deleting Object %p\n", this);
    free(this);    
}

Object *Object_New(void) {
    Object *this = calloc(sizeof(Object), 1);
    this->ref_count = 1;
    printf("constructed Object %p\n", this);
    return this;
}

void Object_AddRef(Object *this, int ref) {
    this->ref_count += ref;
    if (this->ref_count == 0) {
        _Object_Delete(this);
    }
}

void Object_Release(Object *this) {
    Object_AddRef(this, -1);
}

void Object_SetParent(Object *this, Object *parent) {
    
}
