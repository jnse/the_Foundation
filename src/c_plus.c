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

#include "c_plus/defs.h"
#include "c_plus/string.h"
#include "c_plus/time.h"
#include "c_plus/version.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

void init_CPlus(void) {
    init_Garbage();
    printf("[c_Plus] version: %i.%i.%i cstd:%li\n",
           version_CPlus.major, version_CPlus.minor, version_CPlus.patch,
           __STDC_VERSION__);
    const iTime now = now_Time();
    /* Random number generator. */ {
        unsigned seed = nanoSeconds_Time(&now) ^ (integralSeconds_Time(&now) % 1000);
        srand(seed);
        printf("[c_Plus] random seed: %u\n", seed);
    }
    /* Locale. */ {
        const char *lc = getenv("LC_CTYPE");
        if (!lc) lc = getenv("LANG");
        if (!lc) lc = "en_US.utf8";
        setlocale(LC_CTYPE, lc? lc : "en_US.utf8");
        const char *setlc = NULL;
        if (!iCmpStr(setlc = setlocale(LC_CTYPE, NULL), "C")) {
            setlocale(LC_CTYPE, "en_US.UTF-8");
            setlc = setlocale(LC_CTYPE, NULL);
        }
        printf("[c_Plus] locale: %s\n", setlc);
    }
}
