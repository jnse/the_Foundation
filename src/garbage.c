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

#include "lite/garbage.h"
#include "lite/list.h"

#include <stdio.h>
#include <stdlib.h>

iDeclareType(Collected);
struct Impl_Collected {
    iListElement elem;
    void *ptr;
    iDeleteFunc del;
};

static iList *collected; // Should be thread-local...

static void deinit_Garbage(void) {
    recycle_Garbage();
    delete_List(collected);
    collected = NULL;
}

static iList *init_Garbage(void) {
    if (!collected) {
        collected = new_List();
        atexit(deinit_Garbage);
    }
    return collected;
}

void *collect_Garbage(void *ptr, iDeleteFunc del) {
    iCollected *col = malloc(sizeof(iCollected));
    col->ptr = ptr;
    col->del = del;
    pushBack_List(init_Garbage(), col);
    return ptr;
}

void recycle_Garbage(void) {
    if (collected) {
        iReverseForEach(List, i, collected) {
            iCollected *c = (iCollected *) i.value;
            printf("...recycling %p\n", c->ptr);
            c->del(c->ptr);
            free(c);
        }
        clear_List(collected);
    }
}
