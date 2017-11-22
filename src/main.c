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
#include "lite/class.h"
#include "lite/object.h"
#include "lite/counted.h"
#include <stdio.h>

//---------------------------------------------------------------------------------------

typedef struct TestObject_Impl {
    iObject object;
    int value;
}
TestObject;

void TestObject_init(TestObject *d, int value) {
    d->value = value;
}

void TestObject_deinit(iAnyObject *obj) {
    TestObject *d = (TestObject *) obj;
    printf("deinit TestObject: %i\n", d->value);
}

static LITE_DEFINE_CLASS(iClass, TestObject);

TestObject *TestObject_new(int value) {
    TestObject *d = iObject_new(&TestObject_Class);
    TestObject_init(d, value);
    return d;
}

//---------------------------------------------------------------------------------------

typedef struct SuperObject_Impl {
    TestObject base;
    int member;
}
SuperObject;

void SuperObject_init(SuperObject *d, int member) {
    d->member = member;
}

void SuperObject_deinit(iAnyObject *any) {
    SuperObject *d = (SuperObject *) any;
    printf("deinit SuperObject: %i\n", d->member);
}

static LITE_DEFINE_SUBCLASS(iClass, SuperObject, TestObject);

SuperObject *SuperObject_new(int value, int member) {
    SuperObject *d = iObject_new(&SuperObject_Class);
    TestObject_init(&d->base, value);
    SuperObject_init(d, member);
    return d;
}

//---------------------------------------------------------------------------------------

typedef struct TestCounted_Impl {
    iCounted base;
    int value;
}
TestCounted;

void TestCounted_deinit(iAnyCounted *any) {
    TestCounted *d = (TestCounted *) any;
    printf("deinit TestCounted: %i\n", d->value);
}

static LITE_DEFINE_CLASS(iClass, TestCounted);

TestCounted *TestCounted_new(int value) {
    TestCounted *d = iCounted_new(&TestCounted_Class);
    d->value = value;
    return d;
}

//---------------------------------------------------------------------------------------

void printArray(const iArray *list) {
    printf("%4lu %4lu -> %-4lu : %4lu [", iArray_size(list), list->range.start, list->range.end, list->allocSize);
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
    LITE_UNUSED(argc);
    LITE_UNUSED(argv);
    /* Test list insertion and removal. */ {
        printf("Array insertions/removals:\n");
        iArray *list = iArray_new(2);
        printArray(list);
        {
            printf("Iterating the empty list:\n");
            LITE_FOREACH(iArrayConst, i, list) {
                printf("- %p\n", i.value);
            }
        }
        iArray_pushBack(list, "00"); printArray(list);
        iArray_pushBack(list, "11"); printArray(list);
        iArray_pushBack(list, "22"); printArray(list);
        iArray_pushBack(list, "33"); printArray(list);
        iArray_pushBack(list, "44"); printArray(list);
        iArray_pushBack(list, "55"); printArray(list);
        iArray_pushBack(list, "66"); printArray(list);
        iArray_pushBack(list, "77"); printArray(list);
        iArray_pushBack(list, "88"); printArray(list);
        iArray_pushBack(list, "99"); printArray(list);
        iArray_insert(list, 7, "XX"); printArray(list);
        iArray_insert(list, 7, "YY"); printArray(list);
        iArray_insert(list, 8, "ZZ"); printArray(list);
        iArray_pushFront(list, "aa"); printArray(list);
        iArray_pushBack(list, "bb"); printArray(list);
        iArray_pushBack(list, "cc"); printArray(list);
        iArray_sort(list, compareElements); printArray(list);
        iArray_popBack(list); printArray(list);
        iArray_popBack(list); printArray(list);
        iArray_popBack(list); printArray(list);
        iArray_popBack(list); printArray(list);
        iArray_popBack(list); printArray(list);
        iArray_popBack(list); printArray(list);
        iArray_popFront(list); printArray(list);
        iArray_remove(list, 6); printArray(list);
        iArray_remove(list, 5); printArray(list);
        iArray_remove(list, 4); printArray(list);
        iArray_remove(list, 3); printArray(list);
        iArray_remove(list, 2); printArray(list);
        {
            printf("Iterating the list:\n");
            LITE_FOREACH(iArrayConst, i, list) {
                printf("- %p\n", i.value);
            }
        }
        iArray_delete(list);
    }
    /* Test objects. */ {
        TestObject *a = TestObject_new(1);
        TestObject *b = TestObject_new(2);
        SuperObject *c = SuperObject_new(3, 100);
        iObject_setParent(b, a);
        iObject_setParent(c, a);
        printf("Children:\n");
        LITE_FOREACH(iArrayConst, i, &a->object.children.values) {
            printf("- %p\n", *(const void * const *)i.value);
        }
        iObject_delete(a);
    }
    /* Test reference counting. */ {
        TestCounted *a = TestCounted_new(123);
        TestCounted *b = iCounted_ref(a);
        printf("deref a...\n"); iCounted_deref(a);
        printf("deref b...\n"); iCounted_deref(b);
    }
    return 0;
}
