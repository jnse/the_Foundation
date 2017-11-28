/** @file object.c  Reference-counted object.

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

#include "c_plus/object.h"

#include <stdio.h>
#include <stdlib.h>

#if !defined (NDEBUG)
static int totalCount_;
int totalCount_Object(void) {
    return totalCount_;
}
#endif

static void free_Object_(iObject *d) {
    deinit_Object(d);
    iDebug("deleting %s %p\n", d->class->name, d);
    free(d);
#if !defined (NDEBUG)
    totalCount_--;
#endif
}

iAnyObject *new_Object(const iAnyClass *class) {
    iAssert(class != NULL);
    iAssert(((const iClass *) class)->size >= sizeof(iObject));
    iObject *d = malloc(((const iClass *) class)->size);
    d->class = class;
    d->refCount = 1;
#if !defined (NDEBUG)
    totalCount_++;
#endif
    iDebug("constructed %s %p\n", d->class->name, d);
    return d;
}

void deinit_Object(iAnyObject *d) {
    deinit_Class(((iObject *) d)->class, d);
}

iAnyObject *ref_Object(const iAnyObject *any) {
    if (any) {
        iObject *d = iConstCast(iObject *, any);
        d->refCount++;
        return d;
    }
    return NULL;
}

void deref_Object(iAnyObject *any) {
    if (any) {
        iObject *d = (iObject *) any;
        iAssert(d->refCount > 0);
        if (--d->refCount == 0) {
            free_Object_(d);
        }
    }
}

const iClass *class_Object(const iAnyObject *d) {
    if (d) {
        return ((const iObject *) d)->class;
    }
    return NULL;
}
