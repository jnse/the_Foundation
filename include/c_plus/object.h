#pragma once

/** @file c_plus/object.h  Reference-counted object.

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

#include "defs.h"
#include "class.h"

#define iNew(typeName)      new_Object(&Class_##typeName)
#define iRelease(d)         deref_Object(d)
#define iReleaseLater(d)    collect_Object(d)

#define iDefineObjectConstruction(typeName) \
    i##typeName *new_##typeName(void) { \
        i##typeName *d = iNew(typeName); \
        init_##typeName(d); \
        return d; \
    } \
    void delete_##typeName(i##typeName *d) { \
        iRelease(d); \
    }

#define iDefineObjectConstructionArgs(typeName, newArgs, ...) \
    i##typeName *new_##typeName newArgs { \
        i##typeName *d = iNew(typeName); \
        init_##typeName(d, __VA_ARGS__); \
        return d; \
    } \
    void delete_##typeName(i##typeName *d) { \
        iRelease(d); \
    }
/**
 * Reference-counted object that gets deleted only after all references are gone.
 * iObject is used as a base class for many of the objects in c_Plus.
 */
iDeclareType(Object)

struct Impl_Object {
    const iClass *class;
    int refCount;
};

typedef void iAnyObject;

iAnyObject *    new_Object      (const iAnyClass *class);

#define         collect_Object(d)   iCollectDel(d, deref_Object)

void            deinit_Object   (iAnyObject *);

iAnyObject *    ref_Object      (const iAnyObject *);
void            deref_Object    (iAnyObject *);

const iClass *  class_Object    (const iAnyObject *);

#if !defined (NDEBUG)
int             totalCount_Object   (void);
#endif
