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

void init_Threads(void) {
    init_Threads_();
}

void finish_Thread_(iThread *d) {
    d->state = finished_ThreadState;
    iNotifyAudience(d, finished, ThreadFinished);
    iRecycle();
}

static int run_Threads_(void *arg) {
    iThread *d = (iThread *) arg;
    ref_Object(d);
    if (d->flags & terminationEnabled_ThreadFlag) {
#if defined (iHavePthread)
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif
    }
    d->result = d->run(d);
    // Deregister the thread since it's stopping.
    iGuard(runningThreads_, remove_ThreadHash(runningThreads_->value, &d->id));
    d->id = 0;
    // Notify observers that the thread is done.
    finish_Thread_(d);
    deref_Object(d);
    thrd_exit(0); // thread-local data gets deleted
    return 0;
}

//---------------------------------------------------------------------------------------

iDefineClass(Thread)
iDefineObjectConstructionArgs(Thread, (iThreadRunFunc run), run)

void init_Thread(iThread *d, iThreadRunFunc run) {
    d->result = 0;
    d->run = run;
    d->id = 0;
    d->flags = 0;
    d->userData = NULL;
    d->state = created_ThreadState;
    d->finished = NULL;
}

void deinit_Thread(iThread *d) {
    if (d->state == running_ThreadState) {
        iWarning("[Thread] thread %p is being destroyed while still running\n", d);
    }
    delete_Audience(d->finished);
}

void start_Thread(iThread *d) {
    iAssert(d->state == created_ThreadState);
    iLockableThreadHash *threads = init_Threads_();
    d->state = running_ThreadState;
    thrd_create(&d->id, run_Threads_, d);
    iDebug("[Thread] created thread ID %p\n", d->id);
    // Register this thread as a running thread.
    iGuard(threads, insert_ThreadHash(threads->value, &d->id, d));
}

void setUserData_Thread(iThread *d, void *userData) {
    d->userData = userData;
}

void setTerminationEnabled_Thread(iThread *d, iBool enable) {
    iChangeFlags(d->flags, terminationEnabled_ThreadFlag, enable);
}

iBool isRunning_Thread(const iThread *d) {
    return d->state == running_ThreadState;
}

iBool isFinished_Thread(const iThread *d) {
    return d->state == finished_ThreadState;
}

void *userData_Thread(const iThread *d) {
    return d->userData;
}

iThreadResult result_Thread(const iThread *d) {
    if (isRunning_Thread(d)) {
        thrd_join(d->id, NULL);
    }
    iAssert(d->state == finished_ThreadState);
    return d->result;
}

void join_Thread(iThread *d) {
    if (d && isRunning_Thread(d)) {
        thrd_join(d->id, NULL);
        //iAssert(d->state == finished_ThreadState);
    }
}

void terminate_Thread(iThread *d) {
    iAssert(d->flags & terminationEnabled_ThreadFlag);
#if defined (iHavePthread)
    pthread_cancel(d->id);
#endif
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
