#include "lite/object.h"

#include <stdio.h>
#include <stdlib.h>

struct Object_Impl {
    int refCount;
    // Ownership hierarchy (not affected by reference counts).
    Object *parent;
};

static void _Object_delete(Object *self) {
    LITE_ASSERT(self->parent == NULL);
    printf("deleting Object %p\n", self);
    free(self);    
}

Object *Object_new(void) {
    Object *self = calloc(sizeof(Object), 1);
    self->refCount = 1;
    printf("constructed Object %p\n", self);
    return self;
}

void Object_addRef(Object *self, int ref) {
    self->refCount += ref;
    LITE_ASSERT(self->refCount >= 0);
    if (self->refCount <= 0) {
        _Object_delete(self);
    }
}

void Object_release(Object *self) {
    Object_addRef(self, -1);
}

void Object_setParent(Object *self, Object *parent) {
    
}
