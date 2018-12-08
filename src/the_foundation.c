/** @file c_plus.c  Library initialization.

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

#include "the_Foundation/defs.h"
#include "the_Foundation/string.h"
#include "the_Foundation/time.h"
#include "the_Foundation/version.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

static iBool hasBeenInitialized_ = iFalse;

// datagram.c
void init_DatagramThreads(void);

// thread.c
void init_Threads(void);

static void deinit_Foundation_(void) {
    // nothing to do
}

void init_Foundation(void) {
    init_Threads();
    init_Garbage();
    init_DatagramThreads();
    printf("[the_Foundation] version: %i.%i.%i cstd:%li\n",
           version_Foundation.major, version_Foundation.minor, version_Foundation.patch,
           __STDC_VERSION__);
    const iTime now = now_Time();
    /* Random number generator. */ {
        long seed = nanoSeconds_Time(&now) ^ (integralSeconds_Time(&now) % 1000);
        srand((unsigned) seed);
        printf("[the_Foundation] random seed: %ld\n", seed);
    }
    /* Locale. */ {
        setLocale_Foundation();
    }
    hasBeenInitialized_ = iTrue;
    atexit(deinit_Foundation_);
}

iBool isInitialized_Foundation(void) {
    return hasBeenInitialized_;
}

void setLocale_Foundation(void) {
    const char *lc = getenv("LC_CTYPE");
    if (!lc) lc = getenv("LANG");
    if (!lc) lc = "en_US.utf8";
    setlocale(LC_CTYPE, lc? lc : "en_US.utf8");
    const char *setlc = NULL;
    if (!iCmpStr(setlc = setlocale(LC_CTYPE, NULL), "C")) {
        setlocale(LC_CTYPE, "en_US.UTF-8");
        setlc = setlocale(LC_CTYPE, NULL);
    }
    printf("[the_Foundation] locale: %s\n", setlc);    
}

void printMessage_Foundation(FILE *output, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(output, format, args);
    va_end(args);
}
