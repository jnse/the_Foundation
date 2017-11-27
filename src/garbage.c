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

static iList *collected_; // Should be thread-local...

iDeclareType(Collected)
struct Impl_Collected {
    iListElement elem;
    void *ptr;
    iDeleteFunc del;
};

static iCollected *new_Collected_(void *ptr, iDeleteFunc del) {
    iCollected *col = malloc(sizeof(iCollected));
    col->ptr = ptr;
    col->del = del;
    return col;
}

static void delete_Collected_(iCollected *d) {
    printf("...recycling %p\n", d->ptr);
    d->del(d->ptr);
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

void *collect_Garbage(void *ptr, iDeleteFunc del) {
    pushBack_List(init_Garbage_(), new_Collected_(ptr, del));
    return ptr;
}

void recycle_Garbage(void) {
    if (collected_) {
        iReverseForEach(List, i, collected_) {
            delete_Collected_((iCollected *) i.value);
        }
        clear_List(collected_);
    }
}
