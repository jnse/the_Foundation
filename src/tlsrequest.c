#pragma once

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
#include "the_Foundation/socket.h"

struct Impl_TlsRequest {
    iObject                object;
    enum iTlsRequestStatus status;
    iSocket *              socket;
    iBlock                 content;
    iBlock                 result;
};

iDefineObjectConstruction(TlsRequest)

void init_TlsRequest(iTlsRequest *d) {
    d->status = initialized_TlsRequestStatus;
    d->socket = NULL;
    init_Block(&d->content, 0);
    init_Block(&d->result, 0);
}

void deinit_TlsRequest(iTlsRequest *d) {
    iRelease(d->socket);
    deinit_Block(&d->content);
    deinit_Block(&d->result);
}

void setUrl_TlsRequest(iTlsRequest *d, const char *hostName, uint16_t port) {
    if (d->socket) {
        iRelease(d->socket);
    }
    d->socket = new_Socket(hostName, port);
    open_Socket(d->socket);
    /* TODO: Observe socket events. */
}

void setContent_TlsRequest(iTlsRequest *d, const iBlock *content) {
    set_Block(&d->content, content);
}

void submit_TlsRequest(iTlsRequest *d) {

}

enum iTlsRequestStatus status_TlsRequest(const iTlsRequest *d) {
    return d->status;
}

const iBlock *result_TlsRequest(const iTlsRequest *d) {
    return &d->result;
}
