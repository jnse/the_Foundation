/** @file socket.c  TCP socket.

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

#include "c_plus/socket.h"
#include "c_plus/buffer.h"
#include "c_plus/mutex.h"

enum iSocketStatus {
    addressLookup_SocketStatus,
    initialized_SocketStatus,
    connecting_SocketStatus,
    connected_SocketStatus,
    disconnected_SocketStatus,
};

struct Impl_Socket {
    iBuffer output;
    iBuffer input;
    enum iSocketStatus status;
    iAddress *address;
    int fd;
    iMutex mutex;
    // Audiences:
    iAudience *connected;
    iAudience *disconnected;
    iAudience *error;
    iAudience *readyRead;
    iAudience *writeFinished;
};

static iSocketClass Class_Socket;

static void init_Socket_(iSocket *d) {
    init_Mutex(&d->mutex);
    init_Buffer(&d->output);
    init_Buffer(&d->input);
    openEmpty_Buffer(&d->output);
    openEmpty_Buffer(&d->input);
    d->address = NULL;
    d->fd = 0;
}

static void addressLookedUp_Socket_(iAny *any, const iAddress *address) {
    iUnused(address);
    iSocket *d = any;
    // This is being called from another thread.
    iGuardMutex(&d->mutex, {
        if (d->status == addressLookup_SocketStatus) {
            d->status = initialized_SocketStatus;
        }
    });
}

iSocket *newAddress_Socket(const iAddress *address) {
    iSocket *d = iNew(Socket);
    init_Socket_(d);
    waitForFinished_Address(address);
    d->address = ref_Object(address);
    d->status = initialized_SocketStatus;
    return d;
}

void init_Socket(iSocket *d, const char *hostName, uint16_t port) {
    init_Socket_(d);
    d->address = new_Address();
    d->status = addressLookup_SocketStatus;
    insert_Audience(lookupFinished_Address(d->address), d, (iObserverFunc) addressLookedUp_Socket_);
    lookupHost_Address(d->address, hostName, port);
}

void initAddress_Socket(iSocket *d, const iAddress *address) {
    init_Socket_(d);
    d->address = ref_Object(address);
    d->status = initialized_SocketStatus;
}

void deinit_Socket(iSocket *d) {
    iGuardMutex(&d->mutex, {
        deinit_Buffer(&d->output);
        deinit_Buffer(&d->input);
        iReleasePtr(&d->address);
    });
    deinit_Mutex(&d->mutex);
}

iBool open_Socket(iSocket *d) {
    if (isOpen_Socket(d)) return iFalse;
    iGuardMutex(&d->mutex, {
        if (isValid_Address(d->address)) {
            // We already know the address, so
        }
    });
    return iTrue;
}

void close_Socket(iSocket *d) {
    if (isOpen_Socket(d)) {
        flush_Socket(d);

    }
}

iStream *output_Socket(iSocket *d) {
    return stream_Buffer(&d->output);
}

iBool isOpen_Socket(const iSocket *d) {
    iBool open;
    iGuardMutex(&d->mutex, {
        open = (d->status == connecting_SocketStatus ||
                d->status == connected_SocketStatus);
    });
    return open;
}

iMutex *mutex_Socket(iSocket *d) {
    return &d->mutex;
}

static long seek_Socket_(iStream *d, long pos) {
    iUnused(d, pos);
    iAssert(false);
    return 0;
}

static size_t read_Socket_(iSocket *d, size_t size, void *data_out) {
    return readData_Stream(stream_Buffer(&d->input), size, data_out);
}

static size_t write_Socket_(iSocket *d, const void *data, size_t size) {
    writeData_Stream(stream_Buffer(&d->output), data, size);
    cnd_signal(&d->output.dataAvailable);
    return size;
}

static void flush_Socket_(iSocket *d) {

}

static iBeginDefineClass(Socket)
    .super  = &Class_Socket,
    .seek   = seek_Socket_,
    .read   = (size_t (*)(iStream *, size_t, void *))       read_Socket_,
    .write  = (size_t (*)(iStream *, const void *, size_t)) write_Socket_,
    .flush  = (void   (*)(iStream *))                       flush_Socket_,
iEndDefineClass(Socket)
