#pragma once

/** @file lite/list.h  Doubly linked list.

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

#include "defs.h"

/**
 * List does not have ownership of the elements. This means the elements can be
 * any type of object as long as they are derived from ListElement.
 */

iDeclareType(List);
iDeclareType(ListElement);

struct Impl_ListElement {
    iListElement *next;
    iListElement *prev;
};

iList *     new_List    (void);
void        delete_List (iList *);

#define     isEmpty_List(d)     (size_List(d) == 0)

size_t      size_List   (const iList *);
iAny *      front_List  (const iList *);
iAny *      back_List   (const iList *);

void        clear_List      (iList *);

iAny *      pushBack_List   (iList *, iAny *elem);
iAny *      pushFront_List  (iList *, iAny *elem);
iAny *      insertAfter_List(iList *, iAny *after, iAny *elem);
iAny *      insertBefore_List(iList *, iAny *before, iAny *elem);
iAny *      remove_List     (iList *, iAny *elem);
iAny *      popFront_List   (iList *);
iAny *      popBack_List    (iList *);

iDeclareIterator(List, iList *);
iDeclareConstIterator(List, const iList *);
iDeclareReverseIterator(List, iList *);
iDeclareReverseConstIterator(List, const iList *);

struct Iterator_List {
    iList *list;
    iListElement *value;
    iListElement *next;
};
typedef struct Iterator_List ReverseIterator_List;
struct ConstIterator_List {
    const iList *list;
    const iListElement *value;
    const iListElement *next;
};
