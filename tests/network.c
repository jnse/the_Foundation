/**
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

#include <c_plus/address.h>
#include <c_plus/audience.h>
#include <c_plus/commandline.h>
#include <c_plus/string.h>
#include <c_plus/objectlist.h>
#include <c_plus/service.h>
#include <c_plus/socket.h>
#include <c_plus/thread.h>

static void logConnected_(iAny *d, iSocket *sock) {
    printf("Socket %p: connected\n", sock);
}

static void logDisonnected_(iAny *d, iSocket *sock) {
    printf("Socket %p: disconnected\n", sock);
}

static void logWriteFinished_(iAny *d, iSocket *sock) {
    printf("Socket %p: write finished\n", sock);
}

static void logReadyRead_(iAny *d, iSocket *sock) {
    printf("Socket %p: data ready for reading (%zu bytes)\n", sock, receivedBytes_Socket(sock));
}

static void logError_(iAny *d, iSocket *sock, int code, const char *msg) {
    printf("Socket %p: error %i: %s\n", sock, code, msg);
}

static void observeSocket_(iSocket *sock) {
    iConnect(Socket, sock, connected, sock, logConnected_);
    iConnect(Socket, sock, disconnected, sock, logDisonnected_);
    iConnect(Socket, sock, error, sock, logError_);
    iConnect(Socket, sock, readyRead, sock, logReadyRead_);
    iConnect(Socket, sock, writeFinished, sock, logWriteFinished_);
}
static void hostLookedUp(iAny *d, const iAddress *address) {
    iUnused(d);
    if (isValid_Address(address)) {
        iString *ip = toString_Address(address);
        printf("%i IP addresses for %s; chosen: %s \n",
               count_Address(address),
               cstr_String(hostName_Address(address)),
               cstr_String(ip));
        delete_String(ip);
    }
}

static void printMessage_(iAny *any, iSocket *sock) {
    iUnused(any);
    iBlock *data = readAll_Socket(sock);
    printf("%s", constData_Block(data));
    delete_Block(data);
}

static iThreadResult messageReceiver_(iThread *thread) {
    iSocket *sock = userData_Thread(thread);
    iConnect(Socket, sock, readyRead, sock, printMessage_);
    printMessage_(NULL, sock);
    while (isOpen_Socket(sock)) {
        sleep_Thread(0.1);
    }
    iRelease(sock);
    iRelease(thread);
    return 0;
}

static void communicate_(iAny *d, iService *sv, iSocket *sock) {
    iString *addr = toString_Address(address_Socket(sock));
    printf("incoming connecting from %s\n", cstr_String(addr));
    delete_String(addr);
    // Start a new thread to communicate through the socket.
    iThread *receiver = new_Thread(messageReceiver_);
    setUserData_Thread(receiver, ref_Object(sock));
    observeSocket_(sock);
    start_Thread(receiver);
}

int main(int argc, char *argv[]) {
    init_CPlus();
    /* List network interface addresses. */ {
        iObjectList *ifs = networkInterfaces_Address();
        printf("%zu network interfaces:\n", size_ObjectList(ifs));
        iConstForEach(ObjectList, i, ifs) {
            iString *str = toString_Address((const iAddress *) i.object);
            printf("- %s\n", cstr_String(str));
            delete_String(str);
        }
        iRelease(ifs);
    }
    iCommandLine *cmdline = iClob(new_CommandLine(argc, argv));
    // Check the arguments.
    if (contains_CommandLine(cmdline, "s;server")) {
        iService *sv = iClob(new_Service(14666));
        iConnect(Service, sv, incomingAccepted, sv, communicate_);
        if (!open_Service(sv)) {
            puts("Failed to start service");
            return 1;
        }
        puts("Press Enter to quit..."); {
            char line[2];
            fgets(line, sizeof(line), stdin);
        }
    }
    else if (contains_CommandLine(cmdline, "c;client")) {
        iSocket *sock = iClob(new_Socket("localhost", 14666));
        observeSocket_(sock);
        if (!open_Socket(sock)) {
            puts("Failed to connect");
            return 1;
        }
        puts("Type to send a message (empty to quit):");
        for (;;) {
            char buf[200];
            fgets(buf, sizeof(buf), stdin);
            if (strlen(buf) <= 1) break;
            writeData_Socket(sock, buf, strlen(buf));
        }
        puts("Good day!");
    }
    else {
        iConstForEach(CommandLine, i, cmdline) {
            if (i.argType == value_CommandLineArgType) {
                printf("\nLooking up \"%s\"...\n", cstr_String(value_CommandLineConstIterator(&i)));
                iAddress *addr = new_Address();
                iConnect(Address, addr, lookupFinished, addr, hostLookedUp);
                lookupHost_Address(addr, value_CommandLineConstIterator(&i), 0);
                iRelease(addr);
            }
        }
    }
    return 0;
}
