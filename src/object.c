#include "lite/object.h"

#include <stdio.h>
#include <stdlib.h>

struct lObject_Impl {
    int refCount;
    // Ownership hierarchy (not affected by reference counts).
    lObject *parent;
};

static void _Object_delete(lObject *self) {
    LITE_ASSERT(self->parent == NULL);
    printf("deleting Object %p\n", self);
    free(self);    
}

lObject *lObject_new(void) {
    lObject *self = calloc(sizeof(lObject), 1);
    self->refCount = 1;
    printf("constructed Object %p\n", self);
    return self;
}

void lObject_addRef(lObject *self, int ref) {
    self->refCount += ref;
    LITE_ASSERT(self->refCount >= 0);
    if (self->refCount <= 0) {
        _Object_delete(self);
    }
}

void lObject_release(lObject *self) {
    lObject_addRef(self, -1);
}

void lObject_setParent(lObject *self, lObject *parent) {
    
}
