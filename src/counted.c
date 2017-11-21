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

#include "lite/counted.h"

#include <stdio.h>
#include <stdlib.h>

iAnyObject *iCounted_new(size_t size, iDeinitFunc deinit) {
    LITE_ASSERT(size >= sizeof(iCounted));
    iCounted *d = calloc(size, 1);
    d->deinit = deinit;
    d->refCount = 1;
    printf("constructed Counted %p\n", d);
    return d;
}

static void i_Counted_delete(iCounted *d) {
    iCounted_deinit(d);
    printf("deleting Counted %p\n", d);
    free(d);
}

void iCounted_deinit(iAnyCounted *any) {
    iCounted *d = (iCounted *) any;
    LITE_ASSERT(d->refCount == 0);
    if (d->deinit) {
        d->deinit(d);
    }
}

iAnyCounted *iCounted_ref(const iAnyCounted *any) {
    if (any) {
        iCounted *i = LITE_CONST_CAST(iCounted *, any);
        i->refCount++;
        return i;
    }
    return NULL;
}

void iCounted_deref(iAnyCounted *any) {
    if (any) {
        iCounted *d = (iCounted *) any;
        LITE_ASSERT(d->refCount > 0);
        if (--d->refCount <= 0) {
            i_Counted_delete(d);
        }
    }
}
