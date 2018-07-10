/** @file udptest.c  UDP datagram test.

@authors Copyright (c) 2018 Jaakko Keränen <jaakko.keranen@iki.fi>

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

#include <c_plus/address.h>
#include <c_plus/audience.h>
#include <c_plus/commandline.h>
#include <c_plus/string.h>
#include <c_plus/objectlist.h>
#include <c_plus/datagram.h>
#include <c_plus/thread.h>

static void logWriteFinished_(iAny *d, iDatagram *dgm) {
    iUnused(d);
    printf("Datagram %p: write finished\n", dgm);
}

static void logMessage_(iAny *d, iDatagram *dgm) {
    iUnused(d);
    printf("Datagram %p: message ready for reading\n", dgm);
}

static void logError_(iAny *d, iDatagram *dgm, int code, const char *msg) {
    iUnused(d);
    printf("Datagram %p: error %i: %s\n", dgm, code, msg);
}

static void observe_(iDatagram *dgm) {
    iConnect(Datagram, dgm, error, dgm, logError_);
    iConnect(Datagram, dgm, message, dgm, logMessage_);
    iConnect(Datagram, dgm, writeFinished, dgm, logWriteFinished_);
}

static void printMessages_(iAny *any, iDatagram *dgm) {
    iUnused(any);
    iAddress *from;
    iBlock *data;
    while ((data = receive_Datagram(dgm, &from)) != NULL) {
        iString *fromStr = toString_Address(from);
        printf("(%s) %s\n", cstr_String(fromStr), constData_Block(data));
        delete_String(fromStr);
        delete_Block(data);
        iRelease(from);
    }
}

int main(int argc, char *argv[]) {
    init_CPlus();
    iCommandLine *cmdline = iClob(new_CommandLine(argc, argv));
    // Check the arguments.
    if (contains_CommandLine(cmdline, "s;server")) {
        iDatagram *listen = iClob(new_Datagram());
        observe_(listen);
        iConnect(Datagram, listen, message, listen, printMessages_);
        if (!open_Datagram(listen, 14666)) {
            puts("Failed to open socket");
            return 1;
        }
        puts("Press Enter to quit..."); {
            char line[2];
            fgets(line, sizeof(line), stdin);
        }
    }
    else if (contains_CommandLine(cmdline, "c;client")) {
        iDatagram *xmit = iClob(new_Datagram());
        observe_(xmit);
        if (!open_Datagram(xmit, 14667 + rand() % 10000)) {
            puts("Failed to open socket");
            return 2;
        }
        iAddress *dest = new_Address();
        lookupCStr_Address(dest, "", 14666, datagram_SocketType);
        iString *destStr = toString_Address(dest);
        printf("Destination: %s\n", cstr_String(destStr));
        delete_String(destStr);
        puts("Type to send a message (empty to quit):");
        for (;;) {
            char buf[200];
            fgets(buf, sizeof(buf), stdin);
            if (strlen(buf) <= 1) break;
            sendData_Datagram(xmit, buf, strlen(buf), dest);
        }
        puts("Good day!");
    }
    return 0;
}
