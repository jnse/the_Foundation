#pragma once

/** @file thread.h  Thread object.

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
#include "object.h"
#include "time.h"
#include "blockhash.h"
#include "mutex.h"

#include <stdthreads.h>

iDeclareClass(Thread)
iDeclareType(Thread)

typedef thrd_t iThreadId;
typedef int (*iThreadRunFunc)(iThread *);

struct Impl_Thread {
    iObject object;
    iThreadRunFunc run;
    iThreadId id;
    int result;
};

iDeclareObjectConstructionArgs(Thread, iThreadRunFunc run)

iBool   isRunning_Thread    (const iThread *);

/**
 * Returns the result value of the thread. If the thread is still running, the method
 * will block until the result is available. In other words, when the method exits,
 * the thread will always be not running any more.
 *
 * @return Thread result value.
 */
int     result_Thread       (const iThread *);

void    start_Thread        (iThread *);
void    join_Thread         (iThread *);

void        sleep_Thread    (double seconds);
iThread *   current_Thread  (void);

/**
 * Determines the number of CPU cores on the system.
 */
int     idealConcurrentCount_Thread (void);

static inline thrd_t id_Thread(const iThread *d) {
    return d->id;
}

//---------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------

iDeclareBlockHash(ThreadHash, ThreadId, Thread)
iDeclareLockableObject(ThreadHash)
