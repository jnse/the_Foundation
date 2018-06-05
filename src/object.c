/** @file object.c  Reference-counted object.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>

@par License

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
#include "c_plus/audience.h"

#include <stdio.h>
#include <stdlib.h>

#define iObjectSignature    0x4a424f69 // iOBJ

#if 0
#   define iObjectDebug(...)    iDebug(__VA_ARGS__)
#else
#   define iObjectDebug(...)
#endif

#if !defined (NDEBUG)
static atomic_int totalCount_;

int totalCount_Object(void) {
    return totalCount_;
}

void checkSignature_Object(const iAnyObject *d) {
    iAssert(d != NULL);
    iAssert(((const iObject *) d)->__sig == iObjectSignature);
}
#endif

static void free_Object_(iObject *d) {
    deinit_Object(d);
    iObjectDebug("[Object] deleting %s %p\n", d->class->name, d);
#if !defined (NDEBUG)
    d->__sig = 0xdeadbeef;
    totalCount_--;
#endif
    free(d);
}

iAnyObject *new_Object(const iAnyClass *class) {
    iAssert(class != NULL);
    iAssert(((const iClass *) class)->size >= sizeof(iObject));
    iObject *d = malloc(((const iClass *) class)->size);
    d->classObj = class;
    d->refCount = 1;
    d->memberOf = NULL;
#if !defined (NDEBUG)
    d->__sig = iObjectSignature;
    totalCount_++;
#endif
    iObjectDebug("[Object] constructed %s %p\n", d->class->name, d);
    return d;
}

void deinit_Object(iAnyObject *any) {
    iObject *d = (iObject *) any;
    iAssertIsObject(d);
    deinit_Class(d->classObj, d);
    if (d->memberOf) {
        delete_AudienceMember(d->memberOf);
    }
}

iAnyObject *ref_Object(const iAnyObject *any) {
    if (any) {
        iObject *d = iConstCast(iObject *, any);
        iAssertIsObject(d);
        d->refCount++;
        return d;
    }
    return NULL;
}

void deref_Object(const iAnyObject *any) {
    if (any) {
        iObject *d = iConstCast(iObject *, any);
        iAssertIsObject(d);
        iAssert(d->refCount > 0);
        if (--d->refCount == 0) {
            free_Object_(d);
        }
    }
}

const iClass *class_Object(const iAnyObject *d) {
    if (d) {
        iAssertIsObject(d);
        return ((const iObject *) d)->classObj;
    }
    return NULL;
}

iAudienceMember *audienceMember_Object(const iAnyObject *any) {
    if (any) {
        iObject *d = iConstCast(iObject *, any);
        if (!d->memberOf) {
            d->memberOf = new_AudienceMember(d);
        }
        return d->memberOf;
    }
    return NULL;
}
