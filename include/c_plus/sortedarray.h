#pragma once

/** @file c_plus/sortedarray.h  SortedArray of sorted unique values.

Elements of an Array are stored sequentially in a single block of memory. When
elements are inserted, the data is copied into the Array.

SortedArray is an Array whose elements are sorted according to a comparison function.
It is best suited when there is a relatively small amount of insertions/removals
but lots of searches.

@par Complexity

- Insert: O(n)
- Removal: O(n)
- Search: O(log n)

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

#include "array.h"

iDeclareType(SortedArray)

typedef int (*iSortedArrayCompareElemFunc)(const void *, const void *);

struct Impl_SortedArray {
    iArray values;
    iSortedArrayCompareElemFunc cmp;
};

/**
 * Constructs a new sorted array.
 *
 * @param elementSize   Size of array elements (bytes).
 * @param cmp           Element comparison function.
 *
 * @return SortedArray instance.
 */
iSortedArray *  new_SortedArray     (size_t elementSize, iSortedArrayCompareElemFunc cmp);

void            delete_SortedArray  (iSortedArray *);

void            init_SortedArray    (iSortedArray *d, size_t elementSize, iSortedArrayCompareElemFunc cmp);
void            deinit_SortedArray  (iSortedArray *d);

size_t          size_SortedArray    (const iSortedArray *);
iBool           contains_SortedArray(const iSortedArray *, const void *value);
iBool           locate_SortedArray  (const iSortedArray *, const void *value, iRanges *outLoc);

#define         at_SortedArray(d, pos)  at_Array(&(d)->values, pos)
#define         isEmpty_SortedArray(d)  isEmpty_Array(&(d)->values)

void            clear_SortedArray   (iSortedArray *);
iBool           insert_SortedArray  (iSortedArray *, const void *value);
iBool           remove_SortedArray  (iSortedArray *, const void *value);

/** @name Iterators */
///@{
iDeclareIterator(SortedArray, iSortedArray *)
iDeclareConstIterator(SortedArray, const iSortedArray *)
///@}
