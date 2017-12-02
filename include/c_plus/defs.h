#pragma once

/** @file c_plus/defs.h  General definitions.

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

#include <stddef.h>
#include <stdint.h> // prefer to use int{n}_t/uint{n}_t
#include <string.h>

#include "config.h"

#define iFalse  0
#define iTrue   1

#define iInvalidPos     ((size_t) -1)
#define iInvalidSize    ((size_t) -1)

#define iMin(a, b)              ((a) < (b)? (a) : (b))
#define iMax(a, b)              ((a) > (b)? (a) : (b))
#define iClamp(i, low, high)    ((i) < (low)? (low) : (i) > (high)? (high) : (i))
#define iCmp(a, b)              ((a) == (b)? 0 : (a) < (b)? -1 : 1)

// Types.
typedef int iBool;
typedef void iAny;
typedef void iAnyObject;
typedef void (*iDeinitFunc)(iAny *);
typedef void (*iDeleteFunc)(iAny *);

void        init_CPlus  (void);
uint32_t    iCrc32      (const char *data, size_t size);

#define iUnused(var)            ((void)(var))
#define iZap(var)               memset(&(var), 0, sizeof(var));
#define iConstCast(type, ptr)   ((type) (intptr_t) (ptr))
#define iMalloc(typeName)       malloc(sizeof(i##typeName))

#define iDeclareType(typeName)  typedef struct Impl_##typeName i##typeName;

#define iDefineTypeConstruction(typeName) \
    i##typeName *new_##typeName(void) { \
        i##typeName *d = iMalloc(typeName); \
        init_##typeName(d); \
        return d; \
    } \
    void delete_##typeName(i##typeName *d) { \
        deinit_##typeName(d); \
        free(d); \
    }

#define iDefineTypeConstructionArgs(typeName, newArgs, ...) \
    i##typeName *new_##typeName newArgs { \
        i##typeName *d = iMalloc(typeName); \
        init_##typeName(d, __VA_ARGS__); \
        return d; \
    } \
    void delete_##typeName(i##typeName *d) { \
        if (d) { \
            deinit_##typeName(d); \
            free(d); \
        } \
    }

#define iDeclareIterator_(iterType, typeName, container) \
    typedef struct iterType##Impl_##typeName i##typeName##iterType; \
    typedef struct iterType##Impl_##typeName i##typeName##Reverse##iterType; \
    void init_##typeName##iterType(i##typeName##iterType *, container); \
    void next_##typeName##iterType(i##typeName##iterType *); \
    void init_##typeName##Reverse##iterType(i##typeName##Reverse##iterType *, container); \
    void next_##typeName##Reverse##iterType(i##typeName##Reverse##iterType *);

#define iDeclareIterator(typeName, container) \
    iDeclareIterator_(Iterator, typeName, container)

#define iDeclareConstIterator(typeName, container) \
    iDeclareIterator_(ConstIterator, typeName, container)

#define iIterate(typeName, iterType, iterName, container) \
    i##typeName##iterType iterName; \
    for (init_##typeName##iterType(&iterName, container); \
         iterName.value; \
         next_##typeName##iterType(&iterName))

#define iForEach(typeName, iterName, container) \
    iIterate(typeName, Iterator, iterName, container)

#define iReverseForEach(typeName, iterName, container) \
    iIterate(typeName, ReverseIterator, iterName, container)

#define iConstForEach(typeName, iterName, container) \
    iIterate(typeName, ConstIterator, iterName, container)

#define iReverseConstForEach(typeName, iterName, container) \
    iIterate(typeName, ReverseConstIterator, iterName, container)

#if defined (NDEBUG)
#   define iAssert(cond)
#   define iDebug(...)
#else
#   include <stdio.h>
#   include <assert.h>
#   define iAssert(cond)    assert(cond)
#   define iDebug(...)      fprintf(stdout, __VA_ARGS__)
#endif
