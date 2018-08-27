#pragma once

/** @file the_Foundation/ptrset.h  Set of unique pointers.

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

#include "sortedarray.h"

iBeginPublic

typedef iSortedArray iPtrSet;

iDeclareTypeConstruction(PtrSet)

iBool       contains_PtrSet (const iPtrSet *, void *ptr);
iBool       locate_PtrSet   (const iPtrSet *, void *ptr, size_t *pos_out);
void *      at_PtrSet       (iPtrSet *, size_t pos);

#define     isEmpty_PtrSet(d)   isEmpty_SortedArray(d)
#define     size_PtrSet(d)      size_SortedArray(d)

iBool       insert_PtrSet   (iPtrSet *, void *ptr);
iBool       remove_PtrSet   (iPtrSet *, void *ptr);

/** @name Iterators */
///@{
iDeclareIterator(PtrSet, iPtrSet *)
iDeclareConstIterator(PtrSet, const iPtrSet *)
struct IteratorImpl_PtrSet {
    union {
        void **value;
        iArrayIterator iter;
    };
};
struct ConstIteratorImpl_PtrSet {
    union {
        const void **value;
        iArrayConstIterator iter;
    };
};
///@}

iEndPublic
