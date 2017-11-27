/** @file ptrhash.c  Generic hash that uses pointers for keys.

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

#include "c_plus/ptrhash.h"

#include <stdlib.h>

iDefineTypeConstructionArgs(PtrHash, (iPtrHashKeyFunc keyFunc), keyFunc)

void init_PtrHash(iPtrHash *d, iPtrHashKeyFunc keyFunc) {
    init_Hash(&d->hash);
    iAssert(keyFunc);
    d->keyFunc = keyFunc;
}

void deinit_PtrHash(iPtrHash *d) {
    deinit_Hash(&d->hash);
}

iBool contains_PtrHash(const iPtrHash *d, const void *key) {
    return contains_Hash(&d->hash, d->keyFunc(key));
}

iAnyElement *value_PtrHash(iPtrHash *d, const void *key) {
    return value_Hash(&d->hash, d->keyFunc(key));
}

const iAnyElement *constValue_PtrHash(const iPtrHash *d, const void *key) {
    return value_Hash(&d->hash, d->keyFunc(key));
}

void clear_PtrHash(iPtrHash *d) {
    clear_Hash(&d->hash);
}

iAnyElement *insert_PtrHash(iPtrHash *d, iPtrHashElement *element) {
    element->base.key = d->keyFunc(element->key);
    return insert_Hash(&d->hash, &element->base);
}

iAnyElement *remove_PtrHash(iPtrHash *d, const void *key) {
    return remove_Hash(&d->hash, d->keyFunc(key));
}
