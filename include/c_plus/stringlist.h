#pragma once

/** @file c_plus/stringlist.h  List of strings.

StringList is based on a linked list of relatively short StringArrays. Therefore
it is suitable for both small and large numbers of Strings without compromising
performance of sequential access, random-access lookups, insertion, or removal.

     Node <--> Node <--> Node <-->
      |         |         |
    String    String    String
    String    String    String
    String              String
                        String

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
#include "list.h"
#include "string.h"

iDeclareClass(StringList)
iDeclareType(StringList)

struct Impl_StringList {
    iObject object;
    iList list;
    size_t size;
};

/**
 * Constructs a new list of strings.

 * @return StringList object.
 */
iStringList *   new_StringList      (void);

iStringList *   newStrings_StringList       (const iString *, ...);
iStringList *   newStringsCStr_StringList   (const char *, ...);

void            init_StringList     (iStringList *);
void            deinit_StringList   (iStringList *);

void            clear_StringList    (iStringList *);

#define         size_StringList(d)      ((d)->size)
#define         isEmpty_StringList(d)   ((d)->size == 0)

const iString * constAt_StringList  (const iStringList *, size_t pos);
iString *       at_StringList       (iStringList *, size_t pos);

void            pushBack_StringList     (iStringList *, const iString *str);
void            pushBackCStr_StringList (iStringList *, const char *cstr);
void            pushBackCStrN_StringList(iStringList *, const char *cstr, size_t size);

#define         pushBackCStrRange_StringList(d, range)  pushBackCStrN_StringList(d, (range)->start, size_Range(range))

void            pushFront_StringList    (iStringList *, const iString *str);
void            pushFrontCStr_StringList(iStringList *, const char *cstr);
void            popBack_StringList  (iStringList *);
void            popFront_StringList (iStringList *);

void            insert_StringList       (iStringList *, size_t pos, const iString *str);
void            insertCStr_StringList   (iStringList *, size_t pos, const char *cstr);
void            remove_StringList       (iStringList *, size_t pos);

iString *       take_StringList     (iStringList *, size_t pos);

#define         takeFirst_StringList(d)     take_StringList(d, 0)
#define         takeLast_StringList(d)      take_StringList(d, size_StringList(d) - 1)

/** @name Iterators */
///@{
iDeclareIterator(StringList, iStringList *)
struct IteratorImpl_StringList {
    iString *value;
    void *node;
    size_t pos; // in the entire list
    size_t nodePos; // inside the node
    iStringList *list;
};
void            remove_StringListIterator(iStringListIterator *);
iString *       take_StringListIterator(iStringListIterator *);

iDeclareConstIterator(StringList, const iStringList *)
struct ConstIteratorImpl_StringList {
    const iString *value;
    const void *node;
    size_t pos; // in the entire list
    size_t nodePos; // inside the node
    const iStringList *list;
};
///@}
