#include "lite/object.h"

#include <stdio.h>
#include <stdlib.h>

struct i_Object_Impl {
    int refCount;
    // Ownership hierarchy (not affected by reference counts).
    iObject *parent;
};

static void _Object_delete(iObject *d) {
    LITE_ASSERT(d->parent == NULL);
    printf("deleting Object %p\n", d);
    free(d);
}

iObject *iObject_new(void) {
    iObject *d = calloc(sizeof(iObject), 1);
    d->refCount = 1;
    printf("constructed Object %p\n", d);
    return d;
}

void iObject_addRef(iObject *d, int ref) {
    d->refCount += ref;
    LITE_ASSERT(d->refCount >= 0);
    if (d->refCount <= 0) {
        _Object_delete(d);
    }
}

void iObject_release(iObject *d) {
    iObject_addRef(d, -1);
}

void iObject_setParent(iObject *d, iObject *parent) {

}
