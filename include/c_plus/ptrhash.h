#pragma once

/** @file c_plus/ptrhash.h  Generic hash that uses pointers for keys.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>
All rights reserved.

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

#include "hash.h"

iDeclareType(PtrHash);
iDeclareType(PtrHashElement);

typedef iHashKey (*iPtrHashKeyFunc)(const void *key);

struct Impl_PtrHash {
    iHash hash;
    iPtrHashKeyFunc keyFunc;
};

struct Impl_PtrHashElement {
    iHashElement base;
    void *key;
};

/**
 * Construct a new pointer hash.
 *
 * PtrHash does not have ownership of the elements. The class is most suitable as a
 * base for more specialized hashes that actually own the keys and the values.
 *
 * @param keyFunc  Hashing function for generating unique 32-bit integers for each
 *                 key pointer.
 *
 * @return PtrHash instance.
 */
iPtrHash *      new_PtrHash     (iPtrHashKeyFunc keyFunc);

void            delete_PtrHash  (iPtrHash *);

#define         collect_PtrHash(d) iCollectDel(d, delete_PtrHash)

void            init_PtrHash    (iPtrHash *, iPtrHashKeyFunc keyFunc);
void            deinit_PtrHash  (iPtrHash *);

#define         size_PtrHash(d)         size_Hash(&(d)->hash)
#define         isEmpty_PtrHash(d)      isEmpty_Hash(&(d)->hash)

iBool               contains_PtrHash    (const iPtrHash *, const void *key);
iAnyElement *       value_PtrHash       (iPtrHash *, const void *key);
const iAnyElement * constValue_PtrHash  (const iPtrHash *, const void *key);

void            clear_PtrHash   (iPtrHash *);
iAnyElement *   insert_PtrHash  (iPtrHash *, iPtrHashElement *element);
iAnyElement *   remove_PtrHash  (iPtrHash *, const void *key);
