/**
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

#include "lite/array.h"
#include "lite/block.h"
#include "lite/class.h"
#include "lite/counted.h"
#include "lite/garbage.h"
#include "lite/object.h"
#include "lite/string.h"
#include "lite/regexp.h"
#include "lite/ptrarray.h"

#include <stdio.h>
#include <locale.h>

//---------------------------------------------------------------------------------------

typedef struct Impl_TestObject {
    iObject object;
    int value;
}
TestObject;

void init_TestObject(TestObject *d, int value) {
    d->value = value;
}

void deinit_TestObject(iAnyObject *obj) {
    TestObject *d = (TestObject *) obj;
    printf("deinit TestObject: %i\n", d->value);
}

static iDefineClass(iClass, TestObject);

TestObject *new_TestObject(int value) {
    TestObject *d = new_Object(&Class_TestObject);
    init_TestObject(d, value);
    return d;
}

//---------------------------------------------------------------------------------------

typedef struct Impl_SuperObject {
    TestObject base;
    int member;
}
SuperObject;

void init_SuperObject(SuperObject *d, int member) {
    d->member = member;
}

void deinit_SuperObject(iAnyObject *any) {
    SuperObject *d = (SuperObject *) any;
    printf("deinit SuperObject: %i\n", d->member);
}

static iDefineSubclass(iClass, SuperObject, TestObject);

SuperObject *new_SuperObject(int value, int member) {
    SuperObject *d = new_Object(&Class_SuperObject);
    init_TestObject(&d->base, value);
    init_SuperObject(d, member);
    return d;
}

//---------------------------------------------------------------------------------------

typedef struct Impl_TestCounted {
    iCounted base;
    int value;
}
TestCounted;

void deinit_TestCounted(iAnyCounted *any) {
    TestCounted *d = (TestCounted *) any;
    printf("deinit TestCounted: %i\n", d->value);
}

static iDefineClass(iClass, TestCounted);

TestCounted *new_TestCounted(int value) {
    TestCounted *d = new_Counted(&Class_TestCounted);
    d->value = value;
    return d;
}

//---------------------------------------------------------------------------------------

void printArray(const iArray *list) {
    printf("%4lu %4lu -> %-4lu : %4lu [", size_Array(list), list->range.start, list->range.end, list->allocSize);
    for (int i = 0; i < list->allocSize * list->elementSize; ++i) {
        if (i/list->elementSize < list->range.start || i/list->elementSize >= list->range.end)
            printf(" __");
        else
            printf(" %02x", list->data[i]);
    }
    printf(" ]\n");
}

static int compareElements(const void *a, const void *b) {
    return strncmp(a, b, 2);
}

int main(int argc, char *argv[]) {
    iUnused(argc);
    iUnused(argv);
    //setlocale(LC_ALL, "fi_FI.utf-8");
    setlocale(LC_CTYPE, "utf-8");
    /* Test list insertion and removal. */ {
        printf("Array insertions/removals:\n");
        iArray *list = new_Array(2);
        printArray(list);
        {
            printf("Iterating the empty array:\n");
            iForEach(Array, i, list) {
                printf("- %p\n", i.value);
            }
        }
        pushBack_Array(list, "00"); printArray(list);
        pushBack_Array(list, "11"); printArray(list);
        pushBack_Array(list, "22"); printArray(list);
        pushBack_Array(list, "33"); printArray(list);
        pushBack_Array(list, "44"); printArray(list);
        pushBack_Array(list, "55"); printArray(list);
        pushBack_Array(list, "66"); printArray(list);
        pushBack_Array(list, "77"); printArray(list);
        pushBack_Array(list, "88"); printArray(list);
        pushBack_Array(list, "99"); printArray(list);
        insert_Array(list, 7, "XX"); printArray(list);
        insert_Array(list, 7, "YY"); printArray(list);
        insert_Array(list, 8, "ZZ"); printArray(list);
        pushFront_Array(list, "aa"); printArray(list);
        pushBack_Array(list, "bb"); printArray(list);
        pushBack_Array(list, "cc"); printArray(list);
        sort_Array(list, compareElements); printArray(list);
        popBack_Array(list); printArray(list);
        popBack_Array(list); printArray(list);
        popBack_Array(list); printArray(list);
        popBack_Array(list); printArray(list);
        popBack_Array(list); printArray(list);
        popBack_Array(list); printArray(list);
        popFront_Array(list); printArray(list);
        remove_Array(list, 6); printArray(list);
        remove_Array(list, 5); printArray(list);
        remove_Array(list, 4); printArray(list);
        remove_Array(list, 3); printArray(list);
        remove_Array(list, 2); printArray(list);
        {
            printf("Iterating the array:\n");
            iConstForEach(Array, i, list) {
                printf("- %p\n", i.value);
            }
        }
        delete_Array(list);
    }
    /* Array of pointers. */ {
        iPtrArray *par = newPointers_PtrArray("Entry One", "Entry Two", 0);
        printf("Iterating the pointer array:\n");
        iForEach(PtrArray, i, par) {
            printf("- %s\n", i.value);
        }
        delete_PtrArray(par);
    }
    /* Test objects. */ {
        TestObject *a = new_TestObject(1);
        TestObject *b = new_TestObject(2);
        SuperObject *c = new_SuperObject(3, 100);
        setParent_Object(b, a);
        setParent_Object(c, a);
        printf("Children:\n");
        iConstForEach(List, i, a->object.children) {
            printf("- %p\n", i.value);
        }
        delete_Object(b);
        delete_Object(a);
    }
    /* Test reference counting. */ {
        TestCounted *a = new_TestCounted(123);
        TestCounted *b = ref_Counted(a);
        printf("deref a...\n"); deref_Counted(a);
        printf("deref b...\n"); deref_Counted(b);
    }
    /* Test blocks and garbage collector. */ {
        iBlock *a = collect_Block(new_Block(0));
        appendCStr_Block(a, "Hello World");
        appendCStr_Block(a, "!");
        iBlock *b = collect_Block(copy_Block(a));
        iBlock *c = collect_Block(concat_Block(a, b));
        clear_Block(a);
        printf_Block(a, "Hello %i World!", 123);
        printf("Block: %s\n", constData_Block(a));
        printf_Block(a, "What");
        pushBack_Block(a, '?');
        printf("Block: %s %s\n", constData_Block(a), constData_Block(b));
        printf("c-Block: %s\n", constData_Block(c));
        printf("mid: %s\n", constData_Block(collect_Block(mid_Block(b, 3, 4))));
        iRecycle();
    }
    /* Strings. */ {
        iString *s = collect_String(fromCStr_String("A_Äö\U0001f698a"));
        printf("String: %s length: %zu size: %zu\n", cstr_String(s), length_String(s), size_String(s)); {
            iConstForEach(String, i, s) {
                printf(" char: %04x [%lc]\n", i.value, i.value);
            }
        }
        printf("Backwards:\n"); {
            iConstReverseForEach(String, i , s) {
                printf(" char: %04x [%lc]\n", i.value, i.value);
            }
        }
        printf("Starts with: %i %i\n", startsWith_String(s, "a"), startsWithCase_String(s, "a"));
        printf("Ends with: %i %i\n", endsWith_String(s, "a"), endsWithCase_String(s, "A"));
        printf("Mid: %s\n", cstr_String(collect_String(mid_String(s, 3, 1))));
        printf("ö is at: %zu %zu\n", indexOfCStr_String(s, "ö"), indexOf_String(s, u'ö'));
        truncate_String(s, 3);
        printf("Truncated: %s\n", cstr_String(s));
    }
    /* Regular expressions. */ {
        iString *s = fromCStr_String("Hello world Äöäö, there is a \U0001f698 out there.");
        iRegExp *rx = new_RegExp("\\b(THERE|WORLD|äöäö)\\b", caseInsensitive_RegExpOption);
        iRegExpMatch match;
        while (matchString_RegExp(rx, s, &match)) {
            iString *cap = captured_RegExpMatch(&match, 1);
            printf("match: %i -> %i [%s]\n", match.range.start, match.range.end, cstr_String(cap));
            delete_String(cap);
        }
        delete_RegExp(rx);
        delete_String(s);
    }
    /* Compression. */ {
        iString *s = fromCStr_String("Hello world. "
                                     "Hello world. "
                                     "Hello world. "
                                     "Hello world. "
                                     "Hello world. "
                                     "Hello world. "
                                     "Hello world. "
                                     "Hello world. "
                                     "Hello world.");
        iBlock *compr = compress_Block(&s->chars);
        printf("Original: %zu Compressed: %zu\n", size_String(s), size_Block(compr));
        iBlock *restored = decompress_Block(compr);
        printf("Restored %i: %s\n", size_Block(restored), constData_Block(restored));
        delete_Block(restored);
        delete_Block(compr);
    }
    return 0;
}
