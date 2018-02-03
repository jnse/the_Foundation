#pragma once

/** @file c_plus/garbage.h  Garbage collector.

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

#include "defs.h"

#include <stdlib.h>

CPLUS_API void        init_Garbage        (void);

CPLUS_API void        recycle_Garbage     (void);
CPLUS_API void        beginScope_Garbage  (void);
CPLUS_API void        endScope_Garbage    (void);

CPLUS_API iAny *      collect_Garbage     (iAny *ptr, iDeleteFunc del);

#if !defined (__cplusplus)

static inline iAny *iCollectMem(iAny *ptr) { return collect_Garbage(ptr, free); }

#else

template <typename T>
inline T *collect_Garbage(T *ptr, iDeleteFunc del) {
    return reinterpret_cast<T *>(collect_Garbage(reinterpret_cast<iAny *>(ptr), del));
}

template <typename T>
inline T *iCollectMem(T *ptr) {
    return collect_Garbage(ptr, free);
}

#endif

#define iCollectDel(ptr, del)   collect_Garbage(ptr, (iDeleteFunc) (del))

static inline void      iBeginCollect (void) { beginScope_Garbage(); }
static inline void      iEndCollect   (void) { endScope_Garbage(); }
static inline void      iRecycle      (void) { recycle_Garbage(); }
