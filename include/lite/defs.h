#pragma once

/** @file lite/defs.h  General definitions.

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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define iFalse  0
#define iTrue   1

#define iInvalidSize   ((size_t) -1)

#define iMin(a, b)              ((a) < (b)? (a) : (b))
#define iMax(a, b)              ((a) > (b)? (a) : (b))
#define iClamp(i, low, high)    ((i) < (low)? (low) : (i) > (high)? (high) : (i))

// Types.
typedef int             iBool;
typedef uint8_t         iByte;
typedef uint16_t        iChar16;
typedef unsigned int    iUInt;

typedef void iAny;
typedef void iAnyObject;
typedef void (*iDeinitFunc)(iAnyObject *);

#define iUnused(var) ((void)(var))

#define iConstCast(type, ptr) ((type) (intptr_t) (ptr))

#define iDeclareType(typeName) \
    typedef struct Impl_##typeName i##typeName

#define iDeclareIterator_(iterType, typeName, container) \
    typedef struct iterType##_##typeName i##typeName##iterType; \
    void init_##typeName##iterType(i##typeName##iterType *, container); \
    void next_##typeName##iterType(i##typeName##iterType *)

#define iDeclareIterator(typeName, container) \
    iDeclareIterator_(Iterator, typeName, container)

#define iDeclareReverseIterator(typeName, container) \
    iDeclareIterator_(ReverseIterator, typeName, container)

#define iDeclareConstIterator(typeName, container) \
    iDeclareIterator_(ConstIterator, typeName, container)

#define iDeclareReverseConstIterator(typeName, container) \
    iDeclareIterator_(ReverseConstIterator, typeName, container)

#define iForEach(typeName, iterName, container) \
    i##typeName##Iterator iterName; \
    for (init_##typeName##Iterator(&iterName, container); \
         iterName.value != NULL; \
         next_##typeName##Iterator(&iterName))

#define iReverseForEach(typeName, iterName, container) \
    i##typeName##ReverseIterator iterName; \
    for (init_##typeName##ReverseIterator(&iterName, container); \
         iterName.value != NULL; \
         next_##typeName##ReverseIterator(&iterName))

#define iConstForEach(typeName, iterName, container) \
    iForEach(typeName##Const, iterName, container)

#define iConstReverseForEach(typeName, iterName, container) \
    iForEachReverse(typeName##Const, iterName, container)

#if defined (NDEBUG)
#   define iAssert(cond)
#else
#   define iAssert(cond) assert(cond)
#endif
