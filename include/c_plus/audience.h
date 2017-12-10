#pragma once

/** @file c_plus/audience.h  Observer audience.

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
#include "sortedarray.h"

iDeclareType(Audience)
iDeclareType(Observer)

typedef void (*iObserverFunc)(iAny *);

struct Impl_Observer {
    iAny *object;
    iObserverFunc func;
};

struct Impl_Audience {
    iSortedArray observers;
};

iDeclareTypeConstruction(Audience)

void    init_Audience   (iAudience *);
void    deinit_Audience (iAudience *);

iBool   insert_Audience         (iAudience *d, const iObserver *observer);
iBool   remove_Audience         (iAudience *d, const iObserver *observer);
iBool   removeObject_Audience   (iAudience *d, const iAny *object);

/** @name Iterators */
///@{
iDeclareConstIterator(Audience, const iAudience *)

struct ConstIteratorImpl_Audience {
    union {
        const iObserverFunc *value;
        iArrayConstIterator iter;
    };
};
///@}
