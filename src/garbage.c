/** @file garbage.c  Garbage collector.

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

#include "c_plus/garbage.h"
#include "c_plus/list.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdthreads.h>

iDeclareType(GarbageNode)

#define iGarbageNodeMax   16

struct Impl_GarbageNode {
    iListNode node;
    int count;
    struct {
        void *ptr;
        iDeleteFunc del;
    } allocs[iGarbageNodeMax];
};

static inline iBool isEmpty_GarbageNode_(const iGarbageNode *d) { return d->count == 0; }
static inline iBool isFull_GarbageNode_ (const iGarbageNode *d) { return d->count == iGarbageNodeMax; }

static iGarbageNode *new_GarbageNode_(void) {
    iGarbageNode *d = iMalloc(GarbageNode);
    d->count = 0;
    return d;
}

static iBool popBack_GarbageNode_(iGarbageNode *d) {
    if (d->count > 0) {
        if (d->allocs[--d->count].del) {
            d->allocs[d->count].del(d->allocs[d->count].ptr);
            return iTrue;
        }
    }
    return iFalse;
}

static void delete_GarbageNode_(iGarbageNode *d) {
    while (d->count > 0) {
        popBack_GarbageNode_(d);
    }
    free(d);
}

//---------------------------------------------------------------------------------------

iDeclareType(Collected)

static tss_t threadLocalKey_Garbage_;

struct Impl_Collected { // Thread-specific.
    iList collected;
};

static iCollected *new_Collected_(void) {
    iCollected *d = iMalloc(Collected);
    iDebug("[Garbage] created Collected %p\n", d);
    init_List(&d->collected);
    return d;
}

static void recycle_Collected_(iCollected *d) {
    if (!isEmpty_List(&d->collected)) {
        iDebug("[Garbage] recycling %zu allocations\n", size_List(&d->collected));
        iReverseForEach(List, i, &d->collected) {
            delete_GarbageNode_((iGarbageNode *) i.value);
        }
        clear_List(&d->collected);
    }
}

static void delete_Collected_(iCollected *d) {
    recycle_Collected_(d);
    deinit_List(&d->collected);
    free(d);
    iDebug("[Garbage] deleted Collected %p\n", d);
}

static iBool pop_Collected_(iCollected *d) {
    if (isEmpty_List(&d->collected)) {
        return iFalse;
    }
    iGarbageNode *node = back_List(&d->collected);
    if (isEmpty_GarbageNode_(node) && size_List(&d->collected) > 1) {
        popBack_List(&d->collected);
        delete_GarbageNode_(node);
    }
    return popBack_GarbageNode_(back_List(&d->collected));
}

static void push_Collected_(iCollected *d, void *ptr, iDeleteFunc del) {
    iGarbageNode *node = back_List(&d->collected);
    if (!node || isFull_GarbageNode_(node)) {
        pushBack_List(&d->collected, node = new_GarbageNode_());
    }
    node->allocs[node->count].ptr = ptr;
    node->allocs[node->count].del = del;
    node->count++;
}

//---------------------------------------------------------------------------------------

static void deinitForThread_Garbage_(void) {
    iCollected *d = tss_get(threadLocalKey_Garbage_);
    if (d) {
        delete_Collected_(d);
        tss_set(threadLocalKey_Garbage_, NULL);
    }
}

void init_Garbage(void) {
    tss_create(&threadLocalKey_Garbage_, (tss_dtor_t) delete_Collected_);
    atexit(deinitForThread_Garbage_);
}

static iCollected *initForThread_Garbage_(void) {
    iCollected *d = tss_get(threadLocalKey_Garbage_);
    if (!d) {
        tss_set(threadLocalKey_Garbage_, d = new_Collected_());
    }
    return d;
}

static iBool pop_Garbage_(void) {
    return pop_Collected_(initForThread_Garbage_());
}

void *collect_Garbage(void *ptr, iDeleteFunc del) {
    push_Collected_(initForThread_Garbage_(), ptr, del);
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
    iCollected * d = tss_get(threadLocalKey_Garbage_);
    if (d) {
        recycle_Collected_(d);
    }
}
