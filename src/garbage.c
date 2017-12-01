/** @file garbage.c  Garbage collector.

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

#include "c_plus/garbage.h"
#include "c_plus/list.h"

#include <stdio.h>
#include <stdlib.h>

#define iCollectedMax   16

static iList *collected_; // Should be thread-local...

iDeclareType(Collected)

struct Impl_Collected {
    iListNode node;
    int count;
    struct {
        void *ptr;
        iDeleteFunc del;
    } allocs[iCollectedMax];
};

#define isEmpty_Collected_(d)   ((d)->count == 0)
#define isFull_Collected_(d)    ((d)->count == iCollectedMax)

static iCollected *new_Collected_(void) {
    iCollected *d = iMalloc(Collected);
    d->count = 0;
    return d;
}

static iBool popBack_Collected_(iCollected *d) {
    if (d->count > 0) {
        if (d->allocs[--d->count].del) {
            d->allocs[d->count].del(d->allocs[d->count].ptr);
            return iTrue;
        }
    }
    return iFalse;
}

static void delete_Collected_(iCollected *d) {
    while (d->count > 0) {
        popBack_Collected_(d);
    }
    free(d);
}

static void deinit_Garbage_(void) {
    recycle_Garbage();
    delete_List(collected_);
    collected_ = NULL;
}

static iList *init_Garbage_(void) {
    if (!collected_) {
        collected_ = new_List();
        atexit(deinit_Garbage_);
    }
    return collected_;
}

static iBool pop_Garbage_(void) {
    if (!collected_ || isEmpty_List(collected_)) return iFalse;
    iCollected *d = back_List(collected_);
    if (isEmpty_Collected_(d) && size_List(collected_) > 1) {
        popBack_List(collected_);
        delete_Collected_(d);
    }
    return popBack_Collected_(back_List(collected_));
}

void *collect_Garbage(void *ptr, iDeleteFunc del) {
    iList *list = init_Garbage_();
    iCollected *d = back_List(list);
    if (!d || isFull_Collected_(d)) {
        pushBack_List(list, d = new_Collected_());
    }
    d->allocs[d->count].ptr = ptr;
    d->allocs[d->count].del = del;
    d->count++;
    return ptr;
}

void beginScope_Garbage(void) {
    collect_Garbage(NULL, NULL); // marks beginning of scope
}

void endScope_Garbage(void) {
    int count = 0;
    while (pop_Garbage_()) { count++; }
    if (count) {
        iDebug("[Garbage] recycled %i scope allocations\n", count);
    }
}

void recycle_Garbage(void) {
    if (collected_) {
        iDebug("[Garbage] recycling %zu allocations\n", size_List(collected_));
        iReverseForEach(List, i, collected_) {
            delete_Collected_((iCollected *) i.value);
        }
        clear_List(collected_);
    }
}
