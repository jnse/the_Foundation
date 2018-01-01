#pragma once

/** @file c_plus/socket.h  TCP socket.

Socket is a bidirectional non-random-access Stream where data gets written
when flushed.

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

#include "stream.h"
#include "address.h"

typedef iStreamClass iSocketClass;

iDeclareType(Socket)
iDeclareType(Mutex)

iDeclareObjectConstructionArgs(Socket, const char *hostName, uint16_t port)

iSocket *   newAddress_Socket   (const iAddress *address);
iSocket *   newExisting_Socket  (int fd, const void *sockAddr, size_t sockAddrSize);

iBool       open_Socket     (iSocket *);
void        close_Socket    (iSocket *);

iStream *   output_Socket   (iSocket *);

iBool       isOpen_Socket   (const iSocket *);
iMutex *    mutex_Socket    (iSocket *);

size_t      receivedBytes_Socket    (const iSocket *);
const iAddress *address_Socket      (const iSocket *);

static inline void flush_Socket(iSocket *d) { flush_Stream(output_Socket(d)); }
