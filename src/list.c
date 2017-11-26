/** @file list.c  Doubly linked list.

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

#include "c_plus/list.h"

#include <stdlib.h>

#define iListElement_(ptr) ((iListElement *)(ptr))

static iListElement *nextElement_List_(const iList *d, const iListElement *elem) {
    if (!elem || elem->next == &d->root) return NULL;
    return elem->next;
}

static iListElement *prevElement_List_(const iList *d, const iListElement *elem) {
    if (!elem || elem->prev == &d->root) return NULL;
    return elem->prev;
}

iDefineTypeConstruction(List)

void init_List(iList *d) {
    clear_List(d);
}

void deinit_List(iList *d) {
    // List doesn't own any memory, so nothing needs to be deinitialized.
}

size_t size_List(const iList *d) {
    return d? d->size : 0;
}

void *front_List(const iList *d) {
    return d->size > 0? d->root.next : NULL;
}

void *back_List(const iList *d) {
    return d->size > 0? d->root.prev : NULL;
}

void clear_List(iList *d) {
    d->root.next = d->root.prev = &d->root;
    d->size = 0;
}

iAny *pushBack_List(iList *d, iAny *elem) {
    return insertBefore_List(d, &d->root, elem);
}

iAny *pushFront_List(iList *d, iAny *elem) {
    return insertAfter_List(d, &d->root, elem);
}

iAny *insertAfter_List(iList *d, iAny *after, iAny *elem) {
    iAssert(elem);
    if (!after) after = back_List(d);
    iListElement_(elem) ->next = iListElement_(after)->next;
    iListElement_(after)->next->prev = elem;
    iListElement_(elem) ->prev = after;
    iListElement_(after)->next = elem;
    d->size++;
    return elem;
}

iAny *insertBefore_List(iList *d, iAny *before, iAny *elem) {
    iAssert(elem);
    if (!before) before = front_List(d);
    iListElement_(elem)  ->prev = iListElement_(before)->prev;
    iListElement_(before)->prev->next = elem;
    iListElement_(elem)  ->next = before;
    iListElement_(before)->prev = elem;
    d->size++;
    return elem;
}

iAny *popFront_List(iList *d) {
    if (d->size == 0) return NULL;
    return remove_List(d, front_List(d));
}

iAny *popBack_List(iList *d) {
    if (d->size == 0) return NULL;
    return remove_List(d, back_List(d));
}

iAny *remove_List(iList *d, iAny *elem) {
    iAssert(elem);
    iAssert(d->size > 0);
    d->size--;
    iListElement_(elem)->next->prev = iListElement_(elem)->prev;
    iListElement_(elem)->prev->next = iListElement_(elem)->next;
    iListElement_(elem)->next = iListElement_(elem)->prev = NULL;
    return elem;
}

//---------------------------------------------------------------------------------------

#define init_ListIterator_(d, list) \
    { d->list  = list; \
      d->value = nextElement_List_(list, &list->root); \
      d->next  = nextElement_List_(list, d->value); }

#define next_ListIterator_(d) \
    { d->value = d->next; \
      d->next  = nextElement_List_(d->list, d->value); }

void init_ListIterator(iListIterator *d, iList *list) {
    init_ListIterator_(d, list);
}

void next_ListIterator(iListIterator *d) {
    next_ListIterator_(d);
}

void init_ListConstIterator(iListConstIterator *d, const iList *list) {
    init_ListIterator_(d, list);
}

void next_ListConstIterator(iListConstIterator *d) {
    next_ListIterator_(d);
}

//---------------------------------------------------------------------------------------

#define init_ListReverseIterator_(d, list) \
    { d->list  = list; \
      d->value = prevElement_List_(list, &list->root); \
      d->next  = prevElement_List_(list, d->value); }

#define next_ListReverseIterator_(d) \
    { d->value = d->next; \
      d->next  = prevElement_List_(d->list, d->value); }

void init_ListReverseIterator(iListReverseIterator *d, iList *list) {
    init_ListReverseIterator_(d, list);
}

void next_ListReverseIterator(iListReverseIterator *d) {
    next_ListReverseIterator_(d);
}

void init_ListReverseConstIterator(iListReverseConstIterator *d, const iList *list) {
    init_ListReverseIterator_(d, list);
}

void next_ListReverseConstIterator(iListReverseConstIterator *d) {
    next_ListReverseIterator_(d);
}
