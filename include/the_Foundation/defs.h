#pragma once

/** @file the_Foundation/defs.h  General definitions.

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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#if defined (__cplusplus)
#   define iPublic          extern "C"
#   define iBeginPublic     extern "C" {
#   define iEndPublic       }
#else
#   define iPublic
#   define iBeginPublic
#   define iEndPublic
#endif

#define iFalse  false
#define iTrue   true

#define iInvalidPos     ((size_t) -1)
#define iInvalidSize    ((size_t) -1)

#define iMin(a, b)              ((a) < (b)? (a) : (b))
#define iMax(a, b)              ((a) > (b)? (a) : (b))
#define iClamp(i, low, high)    ((i) < (low)? (low) : (i) > (high)? (high) : (i))
#define iCmp(a, b)              ((a) == (b)? 0 : (a) < (b)? -1 : 1)

#define iChangeFlags(var, flags, doSet)   {if (doSet) { (var) |= (flags); } else { (var) &= ~(flags); }}

// Types.
typedef bool iBool;
typedef void iAny;
typedef void iAnyObject;
typedef void (*iDeinitFunc)(iAny *);
typedef void (*iDeleteFunc)(iAny *);

#include "garbage.h"

iPublic void        init_Foundation          (void);
iPublic void        printMessage_Foundation  (FILE *, const char *format, ...);

iPublic uint32_t    iCrc32      (const char *data, size_t size);
iPublic void        iMd5Hash    (const void *data, size_t size, uint8_t md5_out[16]);

#define iUnusedMany_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) \
    ((void)(_0), (void)(_1), (void)(_2), (void)(_3), (void)(_4), \
     (void)(_5), (void)(_6), (void)(_7), (void)(_8), (void)(_9))
#define iUnused(...)            iUnusedMany_(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define iZap(var)               memset(&(var), 0, sizeof(var));
#define iMalloc(typeName)       malloc(sizeof(i##typeName))

#if !defined (__cplusplus)
#   define iConstCast(type, ptr)    ((type) (intptr_t) (ptr))
#   define iFunctionCast(type, ptr) ((type) (ptr))
#else
#   define iConstCast(type, ptr)    const_cast<type>(ptr)
#   define iFunctionCast(type, ptr) reinterpret_cast<type>(ptr)
#endif

#define iDeclareType(typeName)  typedef struct Impl_##typeName i##typeName;

#define iDeclareTypeConstruction(typeName) \
    i##typeName *new_##typeName(void); \
    void delete_##typeName(i##typeName *); \
    static inline i##typeName *collect_##typeName (i##typeName *d) { \
        return iCollectDel(d, delete_##typeName); \
    } \
    void init_##typeName(i##typeName *); \
    void deinit_##typeName(i##typeName *);

#define iDeclareTypeConstructionArgs(typeName, ...) \
    i##typeName *new_##typeName(__VA_ARGS__); \
    void delete_##typeName(i##typeName *); \
    static inline i##typeName *collect_##typeName (i##typeName *d) { \
        return iCollectDel(d, delete_##typeName); \
    } \
    void init_##typeName(i##typeName *, __VA_ARGS__); \
    void deinit_##typeName(i##typeName *);

#define iDefineTypeConstruction(typeName) \
    i##typeName *new_##typeName(void) { \
        i##typeName *d = iMalloc(typeName); \
        init_##typeName(d); \
        return d; \
    } \
    void delete_##typeName(i##typeName *d) { \
        if (d) { \
            deinit_##typeName(d); \
            free(d); \
        } \
    }

#define iDefineStaticTypeConstruction(typeName) \
    static i##typeName *new_##typeName##_(void) { \
        i##typeName *d = iMalloc(typeName); \
        init_##typeName##_(d); \
        return d; \
    } \
    static void delete_##typeName##_(i##typeName *d) { \
        deinit_##typeName##_(d); \
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

#define iForVarArgs(type, var, body) { \
    { body; } \
    va_list iVarArgs_; \
    for (va_start(iVarArgs_, var);;) { \
        var = va_arg(iVarArgs_, type); \
        if (!var) break; \
        { body; } \
    } \
    va_end(iVarArgs_); }

#if defined (NDEBUG)
#   define iAssert(cond)
#   define iDebugOnly(...)  iUnused(__VA_ARGS__)
#else
#   include <assert.h>
#   define iAssert(cond)    assert(cond)
#   define iDebugOnly(...)
#endif

#if defined (iHaveDebugOutput)
#   define iDebug(...)      printMessage_Foundation(stdout, __VA_ARGS__)
#   define iWarning(...)    printMessage_Foundation(stderr, __VA_ARGS__)
#else
#   define iDebug(...)
#   define iWarning(...)
#endif
