/** @file audience.c  Observer audience.

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

#include "c_plus/audience.h"

iDefineTypeConstruction(Audience)

static int cmpObject_Observer_(const void *a, const void *b) {
    const iObserver *x = a, *y = b;
    return iCmp(x->object, y->object);
}

static int cmp_Observer_(const void *a, const void *b) {
    const int cmp = cmpObject_Observer_(a, b);
    if (cmp != 0) return cmp;
    // Same object.
    const iObserver *x = a, *y = b;
    return iCmp((void *) x->func, (void *) y->func);
}

void init_Audience(iAudience *d) {
    init_SortedArray(&d->observers, sizeof(iObserver), cmp_Observer_);
}

void deinit_Audience(iAudience *d) {
    deinit_SortedArray(&d->observers);
}

iBool insert_Audience(iAudience *d, const iObserver *observer) {
    return insert_SortedArray(&d->observers, observer);
}

iBool remove_Audience(iAudience *d, const iObserver *observer) {
    return remove_SortedArray(&d->observers, observer);
}

iBool removeObject_Audience(iAudience *d, const iAny *object) {
    const iRanges range = locateRange_SortedArray(&d->observers, object, cmpObject_Observer_);
    removeRange_SortedArray(&d->observers, &range);
    return !isEmpty_Range(&range);
}

//---------------------------------------------------------------------------------------

void init_AudienceConstIterator(iAudienceConstIterator *d, const iAudience *audience) {
    init_ArrayConstIterator(&d->iter, &audience->observers.values);
}

void next_AudienceConstIterator(iAudienceConstIterator *d) {
    next_ArrayConstIterator(&d->iter);
}
