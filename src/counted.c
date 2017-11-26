/** @file counted.c  Reference-counted object.

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

#include "c_plus/counted.h"

#include <stdio.h>
#include <stdlib.h>

iAnyObject *new_Counted(const iClass *class) {
    iAssert(class != NULL);
    iAssert(class->instanceSize >= sizeof(iCounted));
    iCounted *d = calloc(class->instanceSize, 1);
    d->class = class;
    d->refCount = 1;
    printf("constructed Counted %p\n", d);
    return d;
}

static void delete_Counted_(iCounted *d) {
    deinit_Counted(d);
    printf("deleting Counted %p\n", d);
    free(d);
}

void deinit_Counted(iAnyCounted *any) {
    iCounted *d = (iCounted *) any;
    iAssert(d->refCount == 0);
    deinit_Class(d->class, d);
}

iAnyCounted *ref_Counted(const iAnyCounted *any) {
    if (any) {
        iCounted *i = iConstCast(iCounted *, any);
        i->refCount++;
        return i;
    }
    return NULL;
}

void deref_Counted(iAnyCounted *any) {
    if (any) {
        iCounted *d = (iCounted *) any;
        iAssert(d->refCount > 0);
        if (--d->refCount <= 0) {
            delete_Counted_(d);
        }
    }
}
