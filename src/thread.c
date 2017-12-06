/** @file thread.c  Thread object.

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

#include "c_plus/thread.h"
#include "c_plus/blockhash.h"
#include "c_plus/mutex.h"

iDefinePlainKeyBlockHash(ThreadHash, ThreadId, Thread)

//---------------------------------------------------------------------------------------

iDefineLockableObject(ThreadHash)

static iLockableThreadHash *runningThreads_;

static void deinit_Threads_(void) {
    delete_LockableThreadHash(runningThreads_);
    runningThreads_ = NULL;
}

static iLockableThreadHash *init_Threads_(void) {
    if (!runningThreads_) {
        runningThreads_ = new_LockableThreadHash();
        atexit(deinit_Threads_);
    }
    return runningThreads_;
}

static int run_Threads_(void *arg) {
    iThread *d = (iThread *) arg;
    d->result = d->run(d);
    // Deregister the thread since it's stopping.
    iGuard(runningThreads_, remove_ThreadHash(runningThreads_->value, &d->id));
    d->id = 0;
    return d->result;
}

//---------------------------------------------------------------------------------------

iDefineClass(Thread)
iDefineObjectConstructionArgs(Thread, (iThreadRunFunc run), run)

void init_Thread(iThread *d, iThreadRunFunc run) {
    d->result = 0;
    d->run = run;
    d->id = 0;
}

void deinit_Thread(iThread *d) {
    if (isRunning_Thread(d)) {
        iWarning("[Thread] thread %p is being destroyed while still running\n", d);
    }
}

void start_Thread(iThread *d) {
    iLockableThreadHash *threads = init_Threads_();
    thrd_create(&d->id, run_Threads_, d);
    iDebug("[Thread] created thread ID %p\n", d->id);
    // Register this thread as a running thread.
    iGuard(threads, insert_ThreadHash(threads->value, &d->id, d));
}

iBool isRunning_Thread(const iThread *d) {
    if (!d->id) return iFalse;
    const iLockableThreadHash *threads = init_Threads_();
    iBool running;
    iGuard(threads, running = contains_ThreadHash(threads->value, &d->id));
    return running;
}

int result_Thread(const iThread *d) {
    if (isRunning_Thread(d)) {
        thrd_join(d->id, NULL);
    }
    return d->result;
}

void join_Thread(iThread *d) {
    if (isRunning_Thread(d)) {
        thrd_join(d->id, NULL);
    }
}

void sleep_Thread(double seconds) {
    iTime dur;
    initSeconds_Time(&dur, seconds);
    thrd_sleep(&dur.ts, NULL);
}

iThread *current_Thread(void) {
    iThread *d = NULL;
    const iThreadId cur = thrd_current();
    const iLockableThreadHash *threads = init_Threads_();
    iGuard(threads, d = value_ThreadHash(threads->value, &cur));
    return d;
}
