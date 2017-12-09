/** @file queue.c  Thread-safe queue.

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

#include "c_plus/queue.h"
#include "c_plus/time.h"

iDefineSubclass(Queue, ObjectList)
iDefineObjectConstruction(Queue)

void init_Queue(iQueue *d) {
    init_ObjectList(&d->items);
    init_Mutex(&d->mutex);
    cnd_init(&d->cond);
}

void deinit_Queue(iQueue *d) {
    cnd_destroy(&d->cond);
    deinit_Mutex(&d->mutex);
}

void put_Queue(iQueue *d, iQueueItem *item) {
    iAssert(item != NULL);
    lock_Mutex(&d->mutex);
    pushBack_ObjectList(&d->items, item);
    unlock_Mutex(&d->mutex);
    cnd_signal(&d->cond);
}

iQueueItem *take_Queue(iQueue *d) {
    lock_Mutex(&d->mutex);
    iQueueItem *item = NULL;
    for (;;) {
        item = takeFront_ObjectList(&d->items);
        if (item) break;
        cnd_wait(&d->cond, &d->mutex.mtx);
    }
    unlock_Mutex(&d->mutex);
    return item;
}

iQueueItem *takeTimeout_Queue(iQueue *d, double timeoutSeconds) {
    iTime until;
    initTimeout_Time(&until, timeoutSeconds);
    lock_Mutex(&d->mutex);
    iQueueItem *item = NULL;
    for (;;) {
        item = takeFront_ObjectList(&d->items);
        if (item || cnd_timedwait(&d->cond, &d->mutex.mtx, &until.ts) == thrd_timedout) {
            break;
        }
    }
    unlock_Mutex(&d->mutex);
    return item;
}

iQueueItem *tryTake_Queue(iQueue *d) {
    iQueueItem *item;
    iGuardMutex(&d->mutex, item = takeFront_ObjectList(&d->items));
    return item;
}

size_t size_Queue(const iQueue *d) {
    size_t size;
    iGuardMutex(&d->mutex, size = size_ObjectList(&d->items));
    return size;
}

