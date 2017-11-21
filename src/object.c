/** @file object.c  Object base class.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

<small>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</small>
*/

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
