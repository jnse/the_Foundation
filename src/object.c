#include "lite/object.h"

#include <stdio.h>
#include <stdlib.h>

struct Object_Impl {
    int ref_count;
    // Ownership hierarchy (not affected by reference counts).
    Object *parent;
};

static void _Object_Delete(Object *self) {
    LITE_ASSERT(self->parent == NULL);
    printf("deleting Object %p\n", self);
    free(self);    
}

Object *Object_New(void) {
    Object *self = calloc(sizeof(Object), 1);
    self->ref_count = 1;
    printf("constructed Object %p\n", self);
    return self;
}

void Object_AddRef(Object *self, int ref) {
    self->ref_count += ref;
    LITE_ASSERT(self->ref_count >= 0);
    if (self->ref_count <= 0) {
        _Object_Delete(self);
    }
}

void Object_Release(Object *self) {
    Object_AddRef(self, -1);
}

void Object_SetParent(Object *self, Object *parent) {
    
}
