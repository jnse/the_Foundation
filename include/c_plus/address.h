#pragma once

/** @file c_plus/address.h  Network address.

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

#include "object.h"
#include "audience.h"

iDeclareType(String)

iDeclareClass(Address)
iDeclareType(Address)

typedef void (*iNotifyAddressLookupFinished)(iAny *, const iAddress *);

iDeclareObjectConstruction(Address)

void        init_Address    (iAddress *);
void        deinit_Address  (iAddress *);

iBool       isHostFound_Address (const iAddress *);
iBool       isValid_Address     (const iAddress *);
int         count_Address       (const iAddress *);

iString *       toString_Address    (const iAddress *);

const iString * hostName_Address    (const iAddress *);

void    lookupHost_Address      (iAddress *, const char *hostName, uint16_t port);
iBool   isPending_Address       (const iAddress *);
void    waitForFinished_Address (const iAddress *);

iAudience * lookupFinished_Address  (iAddress *);
