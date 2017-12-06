/** @file platform_linux.c  Linux specific functionality.

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
#include "c_plus/thread.h"
#include "c_plus/string.h"
#include "c_plus/file.h"
#include "c_plus/regexp.h"

int idealConcurrentCount_Thread(void) {
    static int ncpu;
    if (ncpu == 0) {
        iBeginCollect();
        iFile *f = iClob(newCStr_File("/proc/cpuinfo"));
        if (open_File(f, readOnly_FileMode)) {
            const iString *cpuinfo = iClob(newBlock_String(iClob(readAll_File(f))));
            const iRegExp *processorN = iClob(new_RegExp("processor\\s+:\\s+([0-9]+)", 0));
            iRegExpMatch match;
            while (matchString_RegExp(processorN, cpuinfo, &match)) {
                iString *cap = captured_RegExpMatch(&match, 1);
                const int cpuNumber = toInt_String(cap) + 1;
                ncpu = iMax(ncpu, cpuNumber);
                delete_String(cap);
            }
        }
        iEndCollect();
    }
    return ncpu;
}
