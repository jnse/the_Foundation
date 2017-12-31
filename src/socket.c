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
#include "c_plus/thread.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

// address.c
extern void getSockAddr_Address(const iAddress *d, struct sockaddr **addr_out, socklen_t *addrSize_out);

iDeclareType(SocketThread)

enum iSocketStatus {
    addressLookup_SocketStatus,
    initialized_SocketStatus,
    connecting_SocketStatus,
    connected_SocketStatus,
    disconnecting_SocketStatus,
    disconnected_SocketStatus,
};

struct Impl_Socket {
    iBuffer output;
    iBuffer input;
    enum iSocketStatus status;
    iAddress *address;
    int fd;
    iThread *connecting;
    iSocketThread *sender;
    iSocketThread *receiver;
    iCondition allSent;
    iMutex mutex;
    // Audiences:
    iAudience *connected;
    iAudience *disconnected;
    iAudience *error;
    iAudience *readyRead;
    iAudience *writeFinished;
};

static iSocketClass Class_Socket;

//---------------------------------------------------------------------------------------

enum iSocketThreadMode {
    sending_SocketThreadMode,
    receiving_SocketThreadMode,
    exit_SocketThreadMode,
};

iDeclareClass(SocketThread)

struct Impl_SocketThread {
    iThread thread;
    iSocket *socket;
    enum iSocketThreadMode mode;
};

static iThreadResult run_SocketThread_(iThread *thread) {
    iSocketThread *d = (iAny *) thread;
    iMutex *smx = &d->socket->mutex;
    iBlock *inbuf = NULL;
    if (d->mode == receiving_SocketThreadMode) {
        inbuf = new_Block(0x20000);
        collect_Block(inbuf);
    }
    while (d->mode != exit_SocketThreadMode) {
        switch (d->mode) {
            case sending_SocketThreadMode: {
                // Take all the pending data to be sent.
                iBlock *data = NULL;
                iGuardMutex(smx, {
                    if (isEmpty_Buffer(&d->socket->output)) {
                        wait_Condition(&d->socket->output.dataAvailable, smx);
                    }
                    data = readAll_Buffer(&d->socket->output);
                });
                size_t remaining = size_Block(data);
                const char *ptr = constData_Block(data);
                while (remaining > 0) {
                    ssize_t sent = send(d->socket->fd, ptr, remaining, 0);
                    if (sent < 0) {
                        // Error!
                        return errno;
                    }
                    remaining -= sent;
                    ptr += sent;
                }
                delete_Block(data);
                iGuardMutex(smx, {
                    if (isEmpty_Buffer(&d->socket->output)) {
                        signal_Condition(&d->socket->allSent);
                    }
                });
                break;
            }
            case receiving_SocketThreadMode: {
                ssize_t readSize = recv(d->socket->fd, data_Block(inbuf), size_Block(inbuf), 0);
                if (readSize <= 0) {
                    return errno;
                }
                iGuardMutex(smx, {
                    writeData_Buffer(&d->socket->input, constData_Block(inbuf), readSize);
                    signal_Condition(&d->socket->input.dataAvailable);
                });
                break;
            }
            case exit_SocketThreadMode:
                break;
        }
    }
    return 0;
}

static void init_SocketThread(iSocketThread *d, iSocket *socket,
                              enum iSocketThreadMode mode) {
    init_Thread(&d->thread, run_SocketThread_);
    d->socket = socket;
    d->mode = mode;
}

static void deinit_SocketThread(iSocketThread *d) {
    deinit_Thread(&d->thread);
}

static void exit_SocketThread_(iSocketThread *d) {
    d->mode = exit_SocketThreadMode;
    signal_Condition(&d->socket->output.dataAvailable); // if it's waiting...
    join_Thread(&d->thread);
}

iDefineClass(SocketThread)
iDefineObjectConstructionArgs(SocketThread,
                              (iSocket *socket, enum iSocketThreadMode mode),
                              socket, mode)

static inline void start_SocketThread(iSocketThread *d) { start_Thread(&d->thread); }

//---------------------------------------------------------------------------------------

static void setStatus_Socket_(iSocket *d, enum iSocketStatus status) {
    if (d->status != status) {
        d->status = status;
        iDebug("[Socket] %p: state changed to %i (in thread %p)\n", d, status,
               current_Thread());
    }
}

static void init_Socket_(iSocket *d) {
    init_Buffer(&d->output);
    init_Buffer(&d->input);
    openEmpty_Buffer(&d->output);
    openEmpty_Buffer(&d->input);
    d->fd = -1;
    d->address = NULL;
    d->connecting = NULL;
    d->sender = NULL;
    d->receiver = NULL;
    init_Condition(&d->allSent);
    init_Mutex(&d->mutex);
}

static void startThreads_Socket_(iSocket *d) {
    d->sender   = new_SocketThread(d, sending_SocketThreadMode);
    d->receiver = new_SocketThread(d, receiving_SocketThreadMode);
    start_SocketThread(d->sender);
    start_SocketThread(d->receiver);
}

static iThreadResult connectAsync_Socket_(iThread *thd) {
    iSocket *d = userData_Thread(thd);
    struct sockaddr *addr;
    socklen_t addrSize;
    getSockAddr_Address(d->address, &addr, &addrSize);
    const int rc = connect(d->fd, addr, addrSize);
    iGuardMutex(&d->mutex, {
        if (d->status == connecting_SocketStatus) {
            if (rc == 0) {
                setStatus_Socket_(d, connected_SocketStatus);
                startThreads_Socket_(d);
            }
            else {
                setStatus_Socket_(d, disconnected_SocketStatus);
                iWarning("[Socket] connection failed: %s\n", strerror(errno));
            }
        }
        d->connecting = NULL;
    });
    iRelease(thd);
    return rc;
}

static iBool open_Socket_(iSocket *d) {
    // The socket is assumed to be locked already.
    if (isPending_Address(d->address)) {
        setStatus_Socket_(d, connecting_SocketStatus);
        return iTrue; // when address is resolved, the socket will be opened.
    }
    else if (!isValid_Address(d->address)) {
        return iFalse;
    }
    else {
        const iSocketParameters sp = socketParameters_Address(d->address);
        d->fd = socket(sp.family, sp.type, sp.protocol);
        d->connecting = new_Thread(connectAsync_Socket_);
        setStatus_Socket_(d, connecting_SocketStatus);
        setUserData_Thread(d->connecting, d);
        start_Thread(d->connecting);
        return iTrue;
    }
}

static void addressLookedUp_Socket_(iAny *any, const iAddress *address) {
    iUnused(address);
    iSocket *d = any;
    // This is being called from another thread.
    iGuardMutex(&d->mutex, {
        if (d->status == addressLookup_SocketStatus) {
            setStatus_Socket_(d, initialized_SocketStatus);
        }
        else if (d->status == connecting_SocketStatus) {
            open_Socket_(d);
        }
    });
}

static void stopThreads_Socket_(iSocket *d) {
    if (d->sender) {
        exit_SocketThread_(d->sender);
        iReleasePtr(&d->sender);
    }
    if (d->receiver) {
        exit_SocketThread_(d->receiver);
        iReleasePtr(&d->receiver);
    }
}

iSocket *newAddress_Socket(const iAddress *address) {
    iSocket *d = iNew(Socket);
    init_Socket_(d);
    waitForFinished_Address(address);
    d->address = ref_Object(address);
    setStatus_Socket_(d, initialized_SocketStatus);
    return d;
}

iSocket *newExisting_Socket(int fd, const void *sockAddr, size_t sockAddrSize) {
    iSocket *d = iNew(Socket);
    init_Socket_(d);
    d->fd = fd;
    d->address = newSockAddr_Address(sockAddr, sockAddrSize);
    setStatus_Socket_(d, connected_SocketStatus);
    startThreads_Socket_(d);
    return d;
}

void init_Socket(iSocket *d, const char *hostName, uint16_t port) {
    init_Socket_(d);
    d->address = new_Address();
    setStatus_Socket_(d, addressLookup_SocketStatus);
    insert_Audience(lookupFinished_Address(d->address), d,
                    (iObserverFunc) addressLookedUp_Socket_);
    lookupHost_Address(d->address, hostName, port);
}

void deinit_Socket(iSocket *d) {
    close_Socket(d);
    iGuardMutex(&d->mutex, {
        deinit_Buffer(&d->output);
        deinit_Buffer(&d->input);
        iReleasePtr(&d->address);
    });
    deinit_Mutex(&d->mutex);
    deinit_Condition(&d->allSent);
}

iBool open_Socket(iSocket *d) {
    iBool ok;
    lock_Mutex(&d->mutex);
    if (isOpen_Socket(d)) {
        ok = iFalse;
    }
    else {
        ok = open_Socket_(d);
    }
    unlock_Mutex(&d->mutex);
    return ok;
}

void close_Socket(iSocket *d) {
    iGuardMutex(&d->mutex, {
        if (d->status == connected_SocketStatus) {
            flush_Socket(d);
        }
        if (d->fd >= 0) {
            close(d->fd);
            d->fd = -1;
        }
        setStatus_Socket_(d, disconnecting_SocketStatus);
    });
    guardJoin_Thread(d->connecting, &d->mutex);
    iReleasePtr(&d->connecting);
    stopThreads_Socket_(d);
    setStatus_Socket_(d, disconnected_SocketStatus);
    iAssert(d->fd < 0);
    iAssert(!d->sender);
    iAssert(!d->receiver);
    iAssert(!d->connecting);
}

iStream *output_Socket(iSocket *d) {
    return stream_Buffer(&d->output);
}

iMutex *mutex_Socket(iSocket *d) {
    return &d->mutex;
}

iBool isOpen_Socket(const iSocket *d) {
    iBool open;
    iGuardMutex(&d->mutex, {
        open = (d->status == connecting_SocketStatus ||
                d->status == connected_SocketStatus);
    });
    return open;
}

size_t receivedBytes_Socket(const iSocket *d) {
    size_t n;
    iGuardMutex(&d->mutex, n = size_Buffer(&d->input));
    return n;
}

static long seek_Socket_(iStream *d, long pos) {
    iUnused(d, pos);
    iAssert(false); // not allowed
    return 0;
}

static size_t read_Socket_(iSocket *d, size_t size, void *data_out) {
    size_t readSize = 0;
    iGuardMutex(&d->mutex, {
        readSize = readData_Stream(stream_Buffer(&d->input), size, data_out);
    });
    return readSize;
}

static size_t write_Socket_(iSocket *d, const void *data, size_t size) {
    iGuardMutex(&d->mutex, {
        writeData_Stream(stream_Buffer(&d->output), data, size);
        signal_Condition(&d->output.dataAvailable);
    });
    return size;
}

static void flush_Socket_(iSocket *d) {
    iGuardMutex(&d->mutex, {
        if (!isEmpty_Buffer(&d->output)) {
            wait_Condition(&d->allSent, &d->mutex);
        }
    });
}

static iBeginDefineClass(Socket)
    .super  = &Class_Socket,
    .seek   = seek_Socket_,
    .read   = (size_t (*)(iStream *, size_t, void *))       read_Socket_,
    .write  = (size_t (*)(iStream *, const void *, size_t)) write_Socket_,
    .flush  = (void   (*)(iStream *))                       flush_Socket_,
iEndDefineClass(Socket)
