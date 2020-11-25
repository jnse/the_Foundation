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
#include "the_Foundation/time.h"

#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <time.h>

iDeclareType(Context)

#define DEFAULT_BUF_SIZE 1024

static iContext *context_;
static iBool isPrngSeeded_;

struct Impl_Context {
    SSL_CTX *ctx;
};

void init_Context(iContext *d) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    d->ctx = SSL_CTX_new(TLS_client_method());
    if (!d->ctx) {
        iDebug("[TlsRequest] Failed to initialize OpenSSL\n");
        iAssert(d->ctx);
    }
    SSL_CTX_set_verify(d->ctx, SSL_VERIFY_NONE, NULL); /* allow manual verification */
    /* Bug workarounds: https://www.openssl.org/docs/manmaster/man3/SSL_CTX_set_options.html */
    SSL_CTX_set_options(d->ctx, SSL_OP_ALL);
}

void deinit_Context(iContext *d) {
    SSL_CTX_free(d->ctx);
}

iBool isValid_Context(iContext *d) {
    return d->ctx != NULL;
}

iDefineTypeConstruction(Context)

static iBool readAllFromBIO_(BIO *bio, iBlock *out) {
    char buf[DEFAULT_BUF_SIZE];
    int n;
    do {
        n = BIO_read(bio, buf, sizeof(buf));
        if (n > 0) {
            appendData_Block(out, buf, n);
        }
        else if (!BIO_should_retry(bio)) {
            return iFalse;
        }
    } while (n > 0);
    return iTrue;
}

/*----------------------------------------------------------------------------------------------*/

struct Impl_TlsCertificate {
    X509 *cert;
};

iDefineTypeConstruction(TlsCertificate)

void init_TlsCertificate(iTlsCertificate *d) {
    d->cert = NULL;
}

void deinit_TlsCertificate(iTlsCertificate *d) {
    if (d->cert) {
        X509_free(d->cert);
    }
}

static iTlsCertificate *newX509_TlsCertificate_(X509 *cert) {
    iTlsCertificate *d = new_TlsCertificate();
    d->cert = cert;
    return d;
}

iTlsCertificate *newPem_TlsCertificate(const iString *pem) {
    iTlsCertificate *d = new_TlsCertificate();
    BIO *buf = BIO_new_mem_buf(cstr_String(pem), size_String(pem));
    PEM_read_bio_X509(buf, &d->cert, NULL /* no passphrase callback */, "" /* empty passphrase */);
    BIO_free(buf);
    return d;
}

iTlsCertificate *newPemKey_TlsCertificate(const iString *certPem, const iString *keyPem) {
    iTlsCertificate *d = newPem_TlsCertificate(certPem);
    BIO *buf = BIO_new_mem_buf(cstr_String(keyPem), size_String(keyPem));
    EVP_PKEY *pkey = NULL;
    PEM_read_bio_PrivateKey(buf, &pkey, NULL, "");
    X509_set_pubkey(d->cert, pkey);
    EVP_PKEY_free(pkey);
    BIO_free(buf);
    return d;
}

static const iString *findName_(const iTlsCertificateName *names, enum iTlsCertificateNameType type) {
    for (; names->text && names->type; names++) {
        if (names->type == type) {
            return names->text;
        }
    }
    return NULL;
}

static void add_X509Name_(X509_NAME *name, const char *id, enum iTlsCertificateNameType type,
                          const iTlsCertificateName *names) {
    const iString *str = findName_(names, type);
    if (str) {
        X509_NAME_add_entry_by_txt(
            name, id, MBSTRING_UTF8, constData_Block(&str->chars), size_String(str), -1, 0);
    }
}

static void addDomain_X509Name_(X509_NAME *name, enum iTlsCertificateNameType type,
                                const iTlsCertificateName *names) {
    const iString *domain = findName_(names, type);
    if (domain) {
        const iRangecc range = range_String(domain);
        iRangecc comp = iNullRange;
        while (nextSplit_Rangecc(range, ".", &comp)) {
            X509_NAME_add_entry_by_txt(
                name, "DC", MBSTRING_UTF8, (const void *) comp.start, size_Range(&comp), -1, 0);
        }
    }
}

static X509_NAME *makeX509Name_(int kindBit, const iTlsCertificateName *names) {
    X509_NAME *name = X509_NAME_new();
    add_X509Name_(name, "CN",           kindBit | commonName_TlsCertificateNameType, names);
    add_X509Name_(name, "emailAddress", kindBit | emailAddress_TlsCertificateNameType, names);
    add_X509Name_(name, "UID",          kindBit | userId_TlsCertificateNameType, names);
    addDomain_X509Name_(name,           kindBit | domain_TlsCertificateNameType, names);
    add_X509Name_(name, "OU",           kindBit | organizationalUnit_TlsCertificateNameType, names);
    add_X509Name_(name, "O",            kindBit | organization_TlsCertificateNameType, names);
    add_X509Name_(name, "C",            kindBit | country_TlsCertificateNameType, names);
    return name;
}

static void checkErrors_(void) {
    for (unsigned long err = ERR_get_error(); err; err = ERR_get_error()) {
        iDebug("[OpenSSL] %s: %s: %s\n",
               ERR_lib_error_string(err),
               ERR_func_error_string(err),
               ERR_reason_error_string(err));
    }
}

iTlsCertificate *newSelfSignedRSA_TlsCertificate(
    int rsaBits, iDate validUntil, const iTlsCertificateName *names) {
    /* Seed the random number generator. */
    if (!isPrngSeeded_) {
        iTime now;
        initCurrent_Time(&now);
        RAND_seed(&now.ts.tv_nsec, sizeof(now.ts.tv_nsec));
        isPrngSeeded_ = iTrue;
    }
    RSA *rsa = RSA_new();
    BIGNUM *exponent = NULL;
    BN_asc2bn(&exponent, "65537");
    RSA_generate_key_ex(rsa, rsaBits, exponent, NULL);
    BN_free(exponent);
    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, rsa);
    iTlsCertificate *d = new_TlsCertificate();
    d->cert = X509_new();
    X509_set_pubkey(d->cert, pkey);
    /* Set names. */ {
        X509_NAME *issuer = makeX509Name_(issuerBit_TlsCertificateNameType, names);
        X509_set_issuer_name(d->cert, issuer);
        X509_NAME_free(issuer);
        X509_NAME *subject = makeX509Name_(subjectBit_TlsCertificateNameType, names);
        X509_set_subject_name(d->cert, subject);
        X509_NAME_free(subject);
    }
    /* Valid from. */ {
        ASN1_TIME *notBefore = ASN1_TIME_new();
        ASN1_TIME_set(notBefore, time(NULL));
        X509_set1_notBefore(d->cert, notBefore);
        ASN1_TIME_free(notBefore);
    }
    /* Valid until. */ {
        ASN1_TIME *notAfter = ASN1_TIME_new();
        ASN1_TIME_set(notAfter, sinceEpoch_Date(&validUntil));
        X509_set1_notAfter(d->cert, notAfter);
        ASN1_TIME_free(notAfter);
    }
    X509_sign(d->cert, pkey, EVP_sha256());
    checkErrors_();
    EVP_PKEY_free(pkey);
    return d;
}

iBool isEmpty_TlsCertificate(const iTlsCertificate *d) {
    return d->cert == NULL;
}

iString *subject_TlsCertificate(const iTlsCertificate *d) {
    iString *sub = new_String();
    if (d->cert) {
        BIO *buf = BIO_new(BIO_s_mem());
        X509_NAME_print_ex(
            buf, X509_get_subject_name(d->cert), 0, XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB);
        readAllFromBIO_(buf, &sub->chars);
        BIO_free(buf);
    }
    return sub;
}

iString *issuer_TlsCertificate(const iTlsCertificate *d) {
    iString *sub = new_String();
    if (d->cert) {
        BIO *buf = BIO_new(BIO_s_mem());
        X509_NAME_print_ex(
            buf, X509_get_issuer_name(d->cert), 0, XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB);
        readAllFromBIO_(buf, &sub->chars);
        BIO_free(buf);
    }
    return sub;
}

void validUntil_TlsCertificate(const iTlsCertificate *d, iDate *untilDate_out) {
    iAssert(untilDate_out);
    iZap(*untilDate_out);
    if (d->cert) {
        struct tm time;
#if defined (LIBRESSL_VERSION_NUMBER)
        const ASN1_TIME *notAfter = X509_get0_notAfter(d->cert);
        ASN1_time_parse((const char *) ASN1_STRING_get0_data(notAfter),
                        ASN1_STRING_length(notAfter),
                        &time,
                        0);
#else
        ASN1_TIME_to_tm(X509_get0_notAfter(d->cert), &time);
#endif
        initStdTime_Date(untilDate_out, &time);
    }
}

iBool isExpired_TlsCertificate(const iTlsCertificate *d) {
    if (!d->cert) return iTrue;
    return X509_cmp_current_time(X509_get0_notAfter(d->cert)) < 0;
}

iBool verifyDomain_TlsCertificate(const iTlsCertificate *d, iRangecc domain) {
    if (!d->cert) return iFalse;
    return X509_check_host(d->cert, domain.start, size_Range(&domain), 0, NULL) > 0;
}

iBool verifyIp_TlsCertificate(const iTlsCertificate *d, const iString *ipAddress) {
    if (!d->cert || isEmpty_String(ipAddress)) return iFalse;
    int rc = X509_check_ip_asc(d->cert, cstr_String(ipAddress), 0);
    iAssert(rc != -2 /* bad input */);
    return rc > 0;
}

iBool equal_TlsCertificate(const iTlsCertificate *d, const iTlsCertificate *other) {
    if (d->cert == NULL && other->cert == NULL) {
        return iTrue;
    }
    if (d->cert == NULL || other->cert == NULL) {
        return iFalse;
    }
    return X509_cmp(d->cert, other->cert) == 0;
}

iBlock *fingerprint_TlsCertificate(const iTlsCertificate *d) {
    iBlock *sha = new_Block(SHA256_DIGEST_LENGTH);
    if (d->cert) {
        iBlock der;
        init_Block(&der, 0);
        /* Get the DER serialization of the certificate. */ {
            BIO *buf = BIO_new(BIO_s_mem());
            i2d_X509_bio(buf, d->cert);
            readAllFromBIO_(buf, &der);
            BIO_free(buf);
        }
        SHA256(constData_Block(&der), size_Block(&der), data_Block(sha));
        deinit_Block(&der);
    }
    return sha;
}

iString *pem_TlsCertificate(const iTlsCertificate *d) {
    iString *pem = new_String();
    if (d->cert) {
        BIO *buf = BIO_new(BIO_s_mem());
        PEM_write_bio_X509(buf, d->cert);
        readAllFromBIO_(buf, &pem->chars);
        BIO_free(buf);
    }
    return pem;
}

iString *privateKeyPem_TlsCertificate(const iTlsCertificate *d) {
    iString *pem = new_String();
    if (d->cert) {
        BIO *buf = BIO_new(BIO_s_mem());
        PEM_write_bio_PrivateKey(buf, X509_get0_pubkey(d->cert), NULL, NULL, 0, NULL, NULL);
        readAllFromBIO_(buf, &pem->chars);
        BIO_free(buf);
    }
    return pem;
}

/*----------------------------------------------------------------------------------------------*/

struct Impl_TlsRequest {
    iObject          object;
    iMutex           mtx;
    /* Connection. */
    iString *        hostName;
    uint16_t         port;
    iSocket *        socket;
    const iTlsCertificate *clientCert;
    /* Payload and result. */
    iBlock           content;
    iBuffer *        result;
    iTlsCertificate *cert; /* server certificate */
    /* Internal state. */
    volatile enum iTlsRequestStatus status;
    iString *        errorMsg;
    iThread *        thread;
    iBool            notifyReady;
    iBlock  *        incoming;
    iCondition       gotIncoming;
    iMutex           incomingMtx;
    iCondition       requestDone;
    iAudience *      readyRead;
    iAudience *      finished;
    /* OpenSSL state. */
    SSL *            ssl;
    BIO *            rbio; /* we insert incoming encrypted bytes here for SSL to read */
    BIO *            wbio; /* SSL sends encrypted bytes to socket */
    iBlock           sending;
};

iDefineObjectConstruction(TlsRequest)
iDefineAudienceGetter(TlsRequest, readyRead)
iDefineAudienceGetter(TlsRequest, finished)

static void setError_TlsRequest_(iTlsRequest *d, const char *msg);

static void globalCleanup_TlsRequest_(void) {
    if (context_) {
        delete_Context(context_);
    }
}

enum iSSLResult { ok_SSLResult, wantIO_SSLResult, closed_SSLResult, fail_SSLResult };

static enum iSSLResult sslResult_TlsRequest_(iTlsRequest *d, int code) {
    switch (SSL_get_error(d->ssl, code)) {
        case SSL_ERROR_NONE:
            return ok_SSLResult;
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
            return wantIO_SSLResult;
        case SSL_ERROR_ZERO_RETURN:
            return closed_SSLResult;
        case SSL_ERROR_SYSCALL:
        default:
//            fprintf(stderr, "[TlsRequest] SSL_get_error returns %d (code:%d)\n",
//                    SSL_get_error(d->ssl, code), code);
            ERR_print_errors_fp(stderr);
            return fail_SSLResult;
    }
}

static void setStatus_TlsRequest_(iTlsRequest *d, enum iTlsRequestStatus st) {
    iGuardMutex(&d->mtx, if (d->status != st) {
        d->status = st;
        signal_Condition(&d->gotIncoming); /* wake up if sleeping */
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
            setError_TlsRequest_(d, "failure to encrypt data");
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
    init_Mutex(&d->mtx);
    d->hostName = new_String();
    d->port = 0;
    d->socket = NULL;
    d->clientCert = NULL;
    init_Block(&d->content, 0);
    d->result = new_Buffer();
    openEmpty_Buffer(d->result);
    d->cert = NULL;
    d->errorMsg = new_String();
    d->status = initialized_TlsRequestStatus;
    d->thread = NULL;
    d->notifyReady = iFalse;
    d->incoming = new_Block(0);
    init_Mutex(&d->incomingMtx);
    init_Condition(&d->gotIncoming);
    init_Condition(&d->requestDone);
    d->readyRead = NULL;
    d->finished = NULL;
    d->ssl = SSL_new(context_->ctx);
    /* We could also try BIO_s_socket() but all BSD socket related code should be encapsulated
       into the Socket class. */
    d->rbio = BIO_new(BIO_s_mem());
    d->wbio = BIO_new(BIO_s_mem());
    SSL_set_connect_state(d->ssl);
    SSL_set_bio(d->ssl, d->rbio, d->wbio);
    init_Block(&d->sending, 0);
}

void deinit_TlsRequest(iTlsRequest *d) {
    signal_Condition(&d->gotIncoming);
    d->status = finished_TlsRequestStatus;
    if (d->thread) {
        join_Thread(d->thread);
        iRelease(d->thread);
    }
    deinit_Block(&d->sending);
    SSL_free(d->ssl);
    deinit_Condition(&d->requestDone);
    deinit_Condition(&d->gotIncoming);
    deinit_Mutex(&d->incomingMtx);
    delete_Block(d->incoming);
    delete_Audience(d->finished);
    delete_Audience(d->readyRead);
    delete_String(d->errorMsg);
    delete_TlsCertificate(d->cert);
    iRelease(d->result);
    deinit_Block(&d->content);
    iRelease(d->socket);
    delete_String(d->hostName);
    deinit_Mutex(&d->mtx);
}

void setUrl_TlsRequest(iTlsRequest *d, const iString *hostName, uint16_t port) {
    set_String(d->hostName, hostName);
    d->port = port;
}

void setContent_TlsRequest(iTlsRequest *d, const iBlock *content) {
    set_Block(&d->content, content);
}

void setCertificate_TlsRequest(iTlsRequest *d, const iTlsCertificate *cert) {
    d->clientCert = cert;
}

static void appendReceived_TlsRequest_(iTlsRequest *d, const char *buf, size_t len) {
    if (len > 0) {
        iGuardMutex(&d->mtx, {
            writeData_Buffer(d->result, buf, len);
        });
        d->notifyReady = iTrue;
    }
}

static int processIncoming_TlsRequest_(iTlsRequest *d, const char *src, size_t len) {
    /* Note: Runs in the socket thread. */
    char buf[DEFAULT_BUF_SIZE];
    enum iSSLResult status;
    int n;
    do {
        if (len > 0) {
            n = BIO_write(d->rbio, src, len);
            if (n <= 0) {
                return -1; /* assume bio write failure is unrecoverable */
            }
            src += n;
            len -= n;
        }
        if (!SSL_is_init_finished(d->ssl)) {
            if (doHandshake_TlsRequest_(d) == fail_SSLResult) {
                iDebug("[TlsRequest] handshake failure\n");
                setError_TlsRequest_(d, "TLS/SSL handshake failed");
                return -1;
            }
            if (!SSL_is_init_finished(d->ssl)) {
                return 0; /* continue later */
            }
        }
        if (!d->cert) {
            d->cert = newX509_TlsCertificate_(SSL_get_peer_certificate(d->ssl));
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
            setError_TlsRequest_(d, "error while decrypting incoming data");
            return -1;
        }
        if (status == closed_SSLResult && len == 0) {
            setStatus_TlsRequest_(d, finished_TlsRequestStatus); /* even if socket remains open */
        }
    } while (len > 0);
    return 0;
}

static void checkReadyRead_TlsRequest_(iTlsRequest *d) {
    /* All notifications are done from the TlsRequest thread. */
    if (d->notifyReady) {
        d->notifyReady = iFalse;
        iNotifyAudience(d, readyRead, TlsRequestReadyRead);
    }
}

static void gotIncoming_TlsRequest_(iTlsRequest *d, iSocket *socket) {
    iUnused(socket);
    iBlock *data = readAll_Socket(socket);
    lock_Mutex(&d->incomingMtx);
    appendData_Block(d->incoming, constData_Block(data), size_Block(data));
    signal_Condition(&d->gotIncoming);
    unlock_Mutex(&d->incomingMtx);
    delete_Block(data);
}

static iBool readIncoming_TlsRequest_(iTlsRequest *d) {
    lock_Mutex(&d->incomingMtx);
    const iBool didRead = !isEmpty_Block(d->incoming);
    processIncoming_TlsRequest_(d, data_Block(d->incoming), size_Block(d->incoming));
    clear_Block(d->incoming);
    unlock_Mutex(&d->incomingMtx);
    checkReadyRead_TlsRequest_(d);
    return didRead;
}

static iThreadResult run_TlsRequest_(iThread *thread) {
    iTlsRequest *d = userData_Thread(thread);
    doHandshake_TlsRequest_(d);
    for (;;) {
        encrypt_TlsRequest_(d);
        if (!readIncoming_TlsRequest_(d)) {
            lock_Mutex(&d->mtx);
            if (d->status == submitted_TlsRequestStatus) {
                wait_Condition(&d->gotIncoming, &d->mtx);
            }
            else {
//                fprintf(stderr, "[TlsRequest] run loop exiting, status %d\n", d->status);
                unlock_Mutex(&d->mtx);
                break;
            }
            unlock_Mutex(&d->mtx);
        }
    }
    readIncoming_TlsRequest_(d);
    iNotifyAudience(d, finished, TlsRequestFinished);
    iDebug("[TlsRequest] finished\n");
    return 0;
}

static void connected_TlsRequest_(iTlsRequest *d, iSocket *sock) {
    /* The socket has been connected. During this notification the socket remains locked
       so we must start a different thread for carrying out the I/O. */
    iUnused(sock);
    iAssert(!d->thread);
    d->thread = new_Thread(run_TlsRequest_);
    setName_Thread(d->thread, "TlsRequest");
    setUserData_Thread(d->thread, d);
    start_Thread(d->thread);
}

static void disconnected_TlsRequest_(iTlsRequest *d, iSocket *sock) {
    iUnused(sock);
    setStatus_TlsRequest_(d, finished_TlsRequestStatus);
}

static void setError_TlsRequest_(iTlsRequest *d, const char *msg) {
    setCStr_String(d->errorMsg, msg);
    setStatus_TlsRequest_(d, error_TlsRequestStatus);
}

static void handleError_TlsRequest_(iTlsRequest *d, iSocket *sock, int error, const char *msg) {
    iUnused(sock, error);
    setError_TlsRequest_(d, msg);
    if (!d->thread) {
        iNotifyAudience(d, finished, TlsRequestFinished);
    }
}

void submit_TlsRequest(iTlsRequest *d) {
    if (d->status == submitted_TlsRequestStatus) {
        iDebug("[TlsRequest] request already ongoing\n");
        return;
    }
    clear_Buffer(d->result);
    clear_String(d->errorMsg);
    set_Block(&d->sending, &d->content);
    iRelease(d->socket);
    SSL_set1_host(d->ssl, cstr_String(d->hostName));
    /* Server Name Indication for the handshake. */
    SSL_set_tlsext_host_name(d->ssl, cstr_String(d->hostName));
    /* The client certificate. */
    if (d->clientCert) {
        SSL_use_certificate(d->ssl, d->clientCert->cert);
        SSL_use_PrivateKey(d->ssl, X509_get0_pubkey(d->clientCert->cert));
    }
    d->socket = new_Socket(cstr_String(d->hostName), d->port);
    iConnect(Socket, d->socket, connected, d, connected_TlsRequest_);
    iConnect(Socket, d->socket, disconnected, d, disconnected_TlsRequest_);
    iConnect(Socket, d->socket, readyRead, d, gotIncoming_TlsRequest_);
    iConnect(Socket, d->socket, error, d, handleError_TlsRequest_);
    d->status = submitted_TlsRequestStatus;
    if (!open_Socket(d->socket)) {
        d->status = error_TlsRequestStatus;
    }
}

void cancel_TlsRequest(iTlsRequest *d) {
    lock_Mutex(&d->mtx);
    if (d->status == submitted_TlsRequestStatus) {
        unlock_Mutex(&d->mtx);
        close_Socket(d->socket);
    }
    join_Thread(d->thread);
}

void waitForFinished_TlsRequest(iTlsRequest *d) {
    lock_Mutex(&d->mtx);
    if (d->status == submitted_TlsRequestStatus) {
        wait_Condition(&d->requestDone, &d->mtx);
    }
    unlock_Mutex(&d->mtx);
}

const iAddress *address_TlsRequest(const iTlsRequest *d) {
    return d->socket ? address_Socket(d->socket) : NULL;
}

enum iTlsRequestStatus status_TlsRequest(const iTlsRequest *d) {
    return d->status;
}

const iString *errorMessage_TlsRequest(const iTlsRequest *d) {
    return d->errorMsg;
}

const iTlsCertificate *serverCertificate_TlsRequest(const iTlsRequest *d) {
    return d->cert;
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
