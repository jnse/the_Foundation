#pragma once

/** @file c_plus/stringobjecthash.h  Hash that maps Strings to Objects.

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

#include "stringhash.h"
#include "object.h"

iDeclareType(Dictionary);
iDeclareType(DictionaryElement);

struct Impl_Dictionary {
    iObject base;
    iStringHash dict;
};

struct Impl_DictionaryElement {
    iStringHashElement base;
    iObject *value;
};

#if 0
iStringObjectHash * new_StringObjectHash    (void);
void                delete_StringObjectHash (iStringObjectHash *);

#define         collect_PtrHash(d) iCollectDel(d, delete_PtrHash)

void            init_PtrHash    (iPtrHash *, iPtrHashKeyFunc keyFunc);
void            deinit_PtrHash  (iPtrHash *);

#define         size_PtrHash(d)         size_Hash(&(d)->hash)
#define         isEmpty_PtrHash(d)      isEmpty_Hash(&(d)->hash)

iBool                   contains_PtrHash    (const iPtrHash *, const void *key);
iPtrHashElement *       value_PtrHash       (iPtrHash *, const void *key);
const iPtrHashElement * constValue_PtrHash  (const iPtrHash *, const void *key);

void                clear_PtrHash   (iPtrHash *);
iPtrHashElement *   insert_PtrHash  (iPtrHash *, iPtrHashElement *element);
iPtrHashElement *   remove_PtrHash  (iPtrHash *, const void *key);
#endif
