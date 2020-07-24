/** @file the_Foundation/tlsrequest.c  TLS request over a Socket (using OpenSSL)

@authors Copyright (c) 2020 Jaakko Keränen <jaakko.keranen@iki.fi>

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

#include "the_Foundation/tlsrequest.h"
#include "the_Foundation/buffer.h"
#include "the_Foundation/socket.h"
#include "the_Foundation/thread.h"

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

iDeclareType(Context)

#define DEFAULT_BUF_SIZE 512

static iContext *context_;

struct Impl_Context {
    SSL_CTX *ctx;
};

void init_Context(iContext *d) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    d->ctx = SSL_CTX_new(SSLv23_method());
    if (!d->ctx) {
        iDebug("[TlsRequest] Failed to initialize OpenSSL\n");
        iAssert(d->ctx);
    }
    SSL_CTX_set_options(d->ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
}

void deinit_Context(iContext *d) {
    SSL_CTX_free(d->ctx);
}

iBool isValid_Context(iContext *d) {
    return d->ctx != NULL;
}

iDefineTypeConstruction(Context)

struct Impl_TlsRequest {
    iObject                object;
    volatile enum iTlsRequestStatus status;
    iString *              hostName;
    uint16_t               port;
    iSocket *              socket;
    iMutex                 mtx;
    iBlock                 content;
    iBuffer *              result;
    iAtomicInt             notifyReady;
    iAudience *            readyRead;
    iAudience *            finished;
    iCondition             requestDone;
    iThread *              thread;
    /* OpenSSL state. */
    SSL *  ssl;
    BIO *  rbio; /* we insert incoming encrypted bytes here for SSL to read */
    BIO *  wbio; /* SSL sends encrypted bytes to socket */
    iBlock sending;
};

iDefineObjectConstruction(TlsRequest)
iDefineAudienceGetter(TlsRequest, readyRead)
iDefineAudienceGetter(TlsRequest, finished)

static void globalCleanup_TlsRequest_(void) {
    if (context_) {
        delete_Context(context_);
    }
}

enum iSSLResult { ok_SSLResult, wantIO_SSLResult, fail_SSLResult };

static enum iSSLResult sslResult_TlsRequest_(iTlsRequest *d, int code) {
    switch (SSL_get_error(d->ssl, code)) {
        case SSL_ERROR_NONE:
            return ok_SSLResult;
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
            return wantIO_SSLResult;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        default:
            return fail_SSLResult;
    }
}

static void setStatus_TlsRequest_(iTlsRequest *d, enum iTlsRequestStatus st) {
    iGuardMutex(&d->mtx, if (d->status != st) {
        d->status = st;
        if (st == finished_TlsRequestStatus || st == error_TlsRequestStatus) {
            signalAll_Condition(&d->requestDone);
        }
    });
}

static void flushToSocket_TlsRequest_(iTlsRequest *d) {
    char buf[DEFAULT_BUF_SIZE];
    int n;
    do {
        n = BIO_read(d->wbio, buf, sizeof(buf));
        if (n > 0) {
            writeData_Socket(d->socket, buf, n);
        }
        else if (!BIO_should_retry(d->wbio)) {
            iDebug("[TlsRequest] output error (BIO_read)\n");
            setStatus_TlsRequest_(d, error_TlsRequestStatus);
            return;
        }
    } while (n > 0);
}

static enum iSSLResult doHandshake_TlsRequest_(iTlsRequest *d) {
    int n = SSL_do_handshake(d->ssl);
    enum iSSLResult result = sslResult_TlsRequest_(d, n);
    if (result == wantIO_SSLResult) {
        flushToSocket_TlsRequest_(d);
    }
    return result;
}

static iBool encrypt_TlsRequest_(iTlsRequest *d) {
    if (!SSL_is_init_finished(d->ssl)) {
        return iFalse;
    }
    while (!isEmpty_Block(&d->sending)) {
        int n = SSL_write(d->ssl, constData_Block(&d->sending), size_Block(&d->sending));
        enum iSSLResult status = sslResult_TlsRequest_(d, n);
        if (n > 0) {
            remove_Block(&d->sending, 0, n);
            flushToSocket_TlsRequest_(d);
        }
        if (status == fail_SSLResult) {
            iDebug("[TlsRequest] failure to encrypt (SSL_write)\n");
            return iTrue;
        }
        if (n == 0) {
            break;
        }
    }
    return iTrue;
}

void init_TlsRequest(iTlsRequest *d) {
    if (!context_) {
        context_ = new_Context();
        atexit(globalCleanup_TlsRequest_);
    }
    d->status = initialized_TlsRequestStatus;
    d->hostName = new_String();
    d->port = 0;
    d->socket = NULL;
    init_Mutex(&d->mtx);
    init_Block(&d->content, 0);
    d->result = new_Buffer();
    openEmpty_Buffer(d->result);
    set_Atomic(&d->notifyReady, iFalse);
    d->readyRead = NULL;
    d->finished = NULL;
    init_Condition(&d->requestDone);
    d->thread = NULL;
    d->ssl = SSL_new(context_->ctx);
    d->rbio = BIO_new(BIO_s_mem());
    d->wbio = BIO_new(BIO_s_mem());
    init_Block(&d->sending, 0);
    SSL_set_connect_state(d->ssl);
    SSL_set_bio(d->ssl, d->rbio, d->wbio);
}

void deinit_TlsRequest(iTlsRequest *d) {
    d->status = finished_TlsRequestStatus;
    if (d->thread) {
        join_Thread(d->thread);
        iRelease(d->thread);
    }
    deinit_Block(&d->sending);
    SSL_free(d->ssl);
    deinit_Condition(&d->requestDone);
    delete_Audience(d->finished);
    delete_Audience(d->readyRead);
    iRelease(d->result);
    deinit_Block(&d->content);
    deinit_Mutex(&d->mtx);
    iRelease(d->socket);
    delete_String(d->hostName);
}

void setUrl_TlsRequest(iTlsRequest *d, const iString *hostName, uint16_t port) {
    set_String(d->hostName, hostName);
    d->port = port;
}

void setContent_TlsRequest(iTlsRequest *d, const iBlock *content) {
    set_Block(&d->content, content);
}

static void disconnected_TlsRequest_(iAnyObject *obj) {
    iTlsRequest *d = obj;
    setStatus_TlsRequest_(d, finished_TlsRequestStatus);
}

static void appendReceived_TlsRequest_(iTlsRequest *d, const char *buf, size_t len) {
    iGuardMutex(&d->mtx, writeData_Buffer(d->result, buf, len));
    set_Atomic(&d->notifyReady, iTrue);
}

static int processIncoming_TlsRequest_(iTlsRequest *d, const char *src, size_t len) {
    /* Note: Runs in the socket thread. */
    char buf[DEFAULT_BUF_SIZE];
    enum iSSLResult status;
    int n;
    while (len > 0) {
        n = BIO_write(d->rbio, src, len);
        if (n <= 0) {
            return -1; /* assume bio write failure is unrecoverable */
        }
        src += n;
        len -= n;
        if (!SSL_is_init_finished(d->ssl)) {
            if (doHandshake_TlsRequest_(d) == fail_SSLResult) {
                iDebug("[TlsRequest] handshake failure\n");
                return -1;
            }
            if (!SSL_is_init_finished(d->ssl)) {
                return 0; /* continue later */
            }
        }
        /* The encrypted data is now in the input bio so now we can perform actual
           read of unencrypted data. */
        do {
            n = SSL_read(d->ssl, buf, sizeof(buf));
            if (n > 0) {
                appendReceived_TlsRequest_(d, buf, (size_t) n);
            }
        } while (n > 0);

        status = sslResult_TlsRequest_(d, n);
        /* Did SSL request to write bytes? This can happen if peer has requested SSL
           renegotiation. */
        if (status == wantIO_SSLResult) {
            flushToSocket_TlsRequest_(d);
        }
        if (status == fail_SSLResult) {
            return -1;
        }
    }
    return 0;
}

static void readIncoming_TlsRequest_(iAnyObject *obj) {
    iTlsRequest *d = obj;
    iBlock *data = readAll_Socket(d->socket);
    processIncoming_TlsRequest_(d, data_Block(data), size_Block(data));
    delete_Block(data);
}

static void checkReadyRead_TlsRequest_(iTlsRequest *d) {
    /* All notifications are done from the TlsRequest thread. */
    const int isReady = exchange_Atomic(&d->notifyReady, iFalse);
    if (isReady) {
        iNotifyAudience(d, readyRead, TlsRequestReadyRead);
    }
}

static iThreadResult run_TlsRequest_(iThread *thread) {
    iTlsRequest *d = userData_Thread(thread);
    doHandshake_TlsRequest_(d);
    while (d->status == submitted_TlsRequestStatus) {
        encrypt_TlsRequest_(d);
        checkReadyRead_TlsRequest_(d);
        sleep_Thread(0.050); /* TODO: use a condition */
    }
    checkReadyRead_TlsRequest_(d);
    lock_Mutex(&d->mtx); /* maybe we are in the midst of cancelling */
//    ref_Object(d);
    unlock_Mutex(&d->mtx);
    iNotifyAudience(d, finished, TlsRequestFinished);
//    deref_Object(d);
    return 0;
}

static void connected_TlsRequest_(iAnyObject *obj) {
    /* The socket has been connected. During this notification the socket remains locked
       so we must start a different thread for carrying out the I/O. */
    iTlsRequest *d = obj;
    iAssert(!d->thread);
    d->thread = new_Thread(run_TlsRequest_);
    setName_Thread(d->thread, "TlsRequest");
    setUserData_Thread(d->thread, d);
    start_Thread(d->thread);
}

static void handleError_TlsRequest_(iAnyObject *obj, int error, const char *msg) {
    iTlsRequest *d = obj;
    setStatus_TlsRequest_(d, error_TlsRequestStatus);
    close_Socket(d->socket);
}

void submit_TlsRequest(iTlsRequest *d) {
    if (d->status == submitted_TlsRequestStatus) {
        iDebug("[TlsRequest] request already ongoing\n");
        return;
    }
    clear_Buffer(d->result);
    set_Block(&d->sending, &d->content);
    iRelease(d->socket);
    d->socket = new_Socket(cstr_String(d->hostName), d->port);
    iConnect(Socket, d->socket, connected, d, connected_TlsRequest_);
    iConnect(Socket, d->socket, disconnected, d, disconnected_TlsRequest_);
    iConnect(Socket, d->socket, readyRead, d, readIncoming_TlsRequest_);
    iConnect(Socket, d->socket, error, d, handleError_TlsRequest_);
    open_Socket(d->socket);
    d->status = submitted_TlsRequestStatus;
}

void cancel_TlsRequest(iTlsRequest *d) {
    lock_Mutex(&d->mtx);
    if (d->status == submitted_TlsRequestStatus) {
        d->status = error_TlsRequestStatus;
        iDisconnectObject(Socket, d->socket, connected, d);
        iDisconnectObject(Socket, d->socket, disconnected, d);
        iDisconnectObject(Socket, d->socket, readyRead, d);
        iDisconnectObject(Socket, d->socket, error, d);
        close_Socket(d->socket);
        signalAll_Condition(&d->requestDone);
    }
    unlock_Mutex(&d->mtx);
}

void waitForFinished_TlsRequest(iTlsRequest *d) {
    lock_Mutex(&d->mtx);
    if (d->status == submitted_TlsRequestStatus) {
        wait_Condition(&d->requestDone, &d->mtx);
    }
    unlock_Mutex(&d->mtx);
}

enum iTlsRequestStatus status_TlsRequest(const iTlsRequest *d) {
    return d->status;
}

iBlock *readAll_TlsRequest(iTlsRequest *d) {
    iBlock *rd;
    iGuardMutex(&d->mtx, rd = consumeAll_Buffer(d->result));
    return rd;
}

size_t receivedBytes_TlsRequest(const iTlsRequest *d) {
    size_t len;
    iGuardMutex(&d->mtx, len = size_Buffer(d->result));
    return len;
}

iDefineClass(TlsRequest)
