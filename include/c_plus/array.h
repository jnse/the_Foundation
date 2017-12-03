#pragma once

/** @file c_plus/array.h  Array of sequential fixed-size elements.

Array elements are packed sequentially inside a single block of memory. New elements
can be efficiently added to or removed from the start/end of the array.

                 start                          end
                   v                             v
    | ....... | Element | Element | Element | ....... |

@par Complexity

- Push back: amortized O(1)
- Push front: amortized O(1)
- Insert: O(n)
- Pop back: O(1)
- Pop front: O(1)

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

#include "defs.h"
#include "range.h"

struct Impl_Array {
    char *data;
    iRanges range;
    size_t elementSize;
    size_t allocSize;
};

iDeclareType(Array)

/**
 * Constructs a new array.
 *
 * @param elementSize  Size of an element in bytes.
 *
 * @return Empty array.
 */
iArray *    new_Array           (size_t elementSize);

iArray *    newN_Array          (size_t elementSize, const void *value, size_t count);
iArray *    newElements_Array   (size_t elementSize, const void *value, ...);
iArray *    copy_Array          (const iArray *);
void        delete_Array        (iArray *);

void        init_Array      (iArray *, size_t elementSize);
void        deinit_Array    (iArray *);

#define     isEmpty_Array(d)    isEmpty_Range(&(d)->range)
#define     size_Array(d)       size_Range(&(d)->range)
#define     front_Array(d)      at_Array(d, 0)
#define     back_Array(d)       at_Array(d, size_Array(d) - 1)

void *      data_Array      (iArray *);
const void *constData_Array (const iArray *);
void *      at_Array        (const iArray *, size_t pos);
const void *constEnd_Array  (const iArray *);

void        reserve_Array   (iArray *, size_t reservedSize);
void        clear_Array     (iArray *);
void        resize_Array    (iArray *, size_t size);
void        fill_Array      (iArray *, char value);
void        sort_Array      (iArray *, int (*cmp)(const void *, const void *));

#define     set_Array(d, pos, v)        setN_Array      (d, pos, v, 1)
#define     pushBack_Array(d, v)        pushBackN_Array (d, v, 1)
#define     pushFront_Array(d, v)       pushFrontN_Array(d, v, 1)
#define     popBack_Array(d)            popBackN_Array  (d, 1)
#define     popFront_Array(d)           popFrontN_Array (d, 1)
#define     insert_Array(d, pos, v)     insertN_Array   (d, pos, v, 1)
#define     remove_Array(d, pos)        removeN_Array   (d, pos, 1)
#define     removeRange_Array(d, range) removeN_Array   (d, (range)->start, size_Range(range))
#define     take_Array(d, pos, vv)      takeN_Array     (d, pos, vv, 1)

void        setN_Array      (iArray *, size_t pos, const void *value, size_t count);
void        pushBackN_Array (iArray *, const void *value, size_t count);
void        pushFrontN_Array(iArray *, const void *value, size_t count);
size_t      popBackN_Array  (iArray *, size_t count);
size_t      popFrontN_Array (iArray *, size_t count);
size_t      takeN_Array     (iArray *, size_t pos, void *value_out, size_t count);
void        insertN_Array   (iArray *, size_t pos, const void *value, size_t count);
void        removeN_Array   (iArray *, size_t pos, size_t count);
void        move_Array      (iArray *, const iRanges *range, iArray *dest, size_t destPos);

/** @name Iterators */
///@{
iDeclareIterator(Array, iArray *)
iDeclareConstIterator(Array, const iArray *)

size_t index_ArrayIterator(const iArrayIterator *);
size_t index_ArrayConstIterator(const iArrayConstIterator *);
size_t index_ArrayReverseIterator(const iArrayReverseIterator *);
size_t index_ArrayReverseConstIterator(const iArrayReverseConstIterator *);

struct IteratorImpl_Array {
    void *value; // address of element
    iArray *array;
    size_t pos;
};
struct ConstIteratorImpl_Array {
    const void *value; // address of element
    const iArray *array;
    const void *end;
};
///@}
