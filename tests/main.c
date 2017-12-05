/**
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

#include <c_plus/array.h>
#include <c_plus/block.h>
#include <c_plus/class.h>
#include <c_plus/file.h>
#include <c_plus/fileinfo.h>
#include <c_plus/garbage.h>
#include <c_plus/hash.h>
#include <c_plus/map.h>
#include <c_plus/math.h>
#include <c_plus/hash.h>
#include <c_plus/object.h>
#include <c_plus/objectlist.h>
#include <c_plus/ptrarray.h>
#include <c_plus/regexp.h>
#include <c_plus/string.h>
#include <c_plus/stringarray.h>
#include <c_plus/stringlist.h>
#include <c_plus/stringhash.h>
#include <c_plus/time.h>
#include <c_plus/treenode.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

//---------------------------------------------------------------------------------------

iDeclareType(TestNode)
iDeclareStaticClass(TestNode)

struct Impl_TestNode {
    iTreeNode node;
    int value;
};

void init_TestNode(iTestNode *d, int value) {
    d->value = value;
}

void deinit_TestNode(iAnyObject *obj) {
    iTestNode *d = (iTestNode *) obj;
    iDebug("deinit TestNode: %i\n", d->value);
}

static iDefineClass(TestNode)

iTestNode *new_TestNode(int value) {
    iTestNode *d = new_TreeNode(&Class_TestNode);
    init_TestNode(d, value);
    return d;
}

//---------------------------------------------------------------------------------------

iDeclareType(SuperNode)
iDeclareStaticClass(SuperNode)

struct Impl_SuperNode {
    iTestNode testNode;
    int member;
};

void init_SuperNode(iSuperNode *d, int member) {
    d->member = member;
}

void deinit_SuperNode(iAnyObject *any) {
    iSuperNode *d = (iSuperNode *) any;
    iDebug("deinit SuperNode: %i\n", d->member);
}

static iDefineSubclass(SuperNode, TestNode)

iSuperNode *new_SuperNode(int value, int member) {
    iSuperNode *d = new_TreeNode(&Class_SuperNode);
    init_TestNode(&d->testNode, value);
    init_SuperNode(d, member);
    return d;
}

//---------------------------------------------------------------------------------------

iDeclareType(TestObject)
iDeclareStaticClass(TestObject)

struct Impl_TestObject {
    iObject object;
    int value;
};

static void deinit_TestObject(iAnyObject *any) {
    iTestObject *d = (iTestObject *) any;
    iDebug("deinit TestObject: %i\n", d->value);
}

static iDefineClass(TestObject)

iTestObject *new_TestObject(int value) {
    iTestObject *d = iNew(TestObject);
    d->value = value;
    return d;
}

//---------------------------------------------------------------------------------------

void printArray(const iArray *list) {
    printf("%4lu %4lu -> %-4lu : %4lu [", size_Array(list), list->range.start, list->range.end, list->allocSize);
    for (int i = 0; i < list->allocSize * list->elementSize; ++i) {
        if (i/list->elementSize < list->range.start || i/list->elementSize >= list->range.end)
            printf(" _");
        else
            printf(" %c", list->data[i]);
    }
    printf(" ]\n");
}

static int compareTextElements(const void *a, const void *b) {
    return iCmpStrN(a, b, 2);
}

static int compareIntegers(iMapKey a, iMapKey b) {
    return iCmp(a, b);
}

static int compareTestObjects(const void *a, const void *b) {
    const iTestObject *x = object_ObjectListNode(a);
    const iTestObject *y = object_ObjectListNode(b);
    iAssert(a != b);
    iDebug("comparing: (%p) %i and (%p) %i\n", x, x->value, y, y->value);
    return iCmp(x->value, y->value);
}

int main(int argc, char *argv[]) {
    iUnused(argc);
    iUnused(argv);
    init_CPlus();
    /* Test time and date. */ {
        const iTime now = now_Time();
        iDate date;
        init_Date(&date, &now);
        printf("Today is %i-%02i-%02i (week day %i) and the time is %02i:%02i:%02i.%li (GMT offset: %li mins)\n",
               date.year, date.month, date.day, date.dayOfWeek,
               date.hour, date.minute, date.second, date.nsecs,
               date.gmtOffsetSeconds/60);
    }
    /* File information. */ {
        iBeginCollect();
        iForEach(DirFileInfo, i, iReleaseLater(newCStr_DirFileInfo("."))) {
            printf("%10li %s %12li %s\n",
                   size_FileInfo(i.value),
                   isDirectory_FileInfo(i.value)? "(dir)" : "     ",
                   lastModified_FileInfo(i.value).ts.tv_sec,
                   cstrPath_FileInfo(i.value));
        }
        iEndCollect();
    }
    /* Test array insertion and removal. */ {
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
        pushBackN_Array(list, "11223344", 4); printArray(list);
        pushBack_Array(list, "55"); printArray(list);
        pushBack_Array(list, "66"); printArray(list);
        pushBack_Array(list, "77"); printArray(list);
        pushBack_Array(list, "88"); printArray(list);
        pushBack_Array(list, "99"); printArray(list);
        insertN_Array(list, 7, "XXYY", 2); printArray(list);
        insert_Array(list, 8, "ZZ"); printArray(list);
        pushFront_Array(list, "aa"); printArray(list);
        pushBack_Array(list, "bb"); printArray(list);
        pushBack_Array(list, "cc"); printArray(list);
        sort_Array(list, compareTextElements); printArray(list);
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
    /* Test an array of pointers. */ {
        iPtrArray *par = newPointers_PtrArray("Entry One", "Entry Two", NULL);
        printf("Iterating the pointer array:\n");
        iConstForEach(PtrArray, i, par) {
            printf("- %s\n", i.ptr);
        }
        delete_PtrArray(par);
    }
    /* Test an array of strings. */ {
        iStringArray *sar = newStringsCStr_StringArray("Hello World", "Another string", "3rd text", NULL);
        printf("StringArray contents:\n");
        iConstForEach(StringArray, i, sar) {
            printf("%4zu: \"%s\"\n", index_StringArrayConstIterator(&i), cstr_String(*i.value));
        }
        iRelease(sar);
    }
    /* Test a list of strings. */ {
        iFile *file = newCStr_File("/Users/jaakko/src/cplus/CMakeLists.txt");
        if (open_File(file, text_FileFlag)) {
            iStringList *list = readLines_File(file);
            iConstForEach(StringList, i, list) {
                printf("%4zu: \"%s\"\n", i.pos, cstr_String(i.value));
            }
            iRelease(list);
        }
        iRelease(file);
    }
    /* Test an object list. */ {
        iObjectList *olist = new_ObjectList();
        pushBack_ObjectList(olist, iReleaseLater(new_TestObject(500)));
        pushBack_ObjectList(olist, iReleaseLater(new_TestObject(400)));
        pushFront_ObjectList(olist, iReleaseLater(new_TestObject(600)));
        //sort_List(list_ObjectList(olist), compareTestObjects);
        printf("List of objects:");
        iConstForEach(ObjectList, i, olist) {
            printf("%4i", ((const iTestObject *) i.object)->value);
        }
        printf("\n");
        iRelease(olist);
    }
    /* Test a string hash. */ {
        iStringHash *h = new_StringHash();
        insertValuesCStr_StringHash(h,
              "one", iReleaseLater(new_TestObject(1000)),
              "two", iReleaseLater(new_TestObject(1001)), NULL);
        printf("Hash has %zu nodes:\n", size_StringHash(h));
        iConstForEach(StringHash, i, h) {
            printf("  %s: %i\n",
                   cstr_String(key_StringHashConstIterator(&i)),
                   ((iTestObject *) i.value->object)->value);
        }
        iRelease(h);
    }
    /* Test a hash. */ {
        iHash *h = new_Hash();
        for (int i = 0; i < 10; ++i) {
            iHashNode *node = iCollect(iMalloc(HashNode));
            node->key = i;
            insert_Hash(h, node);
        }
        printf("Hash iteration:\n");
        int counter = 0;
        iForEach(Hash, i, h) {
            printf("%4i: %i\n", counter++, i.value->key);
        }
        delete_Hash(h);
    }
    /* Test a map. */ {
        iBeginCollect();
        printf("Testing a map.\n");
        iMap *map = new_Map(compareIntegers);
        for (int i = 0; i < 20; ++i) {
            iMapNode *node = iMalloc(MapNode);
            node->key = iRandom(0, 100);
            iMapNode *old = insert_Map(map, node);
            if (old) free(old);
        }
        printf("Keys: ["); {
            iForEach(Map, i, map) {
                printf(" %2lli", i.value->key);
                iCollect(i.value);
            }
        }
        printf(" ]\n");
        printf("Keys in reverse: ["); {
            iReverseConstForEach(Map, i, map) {
                printf(" %2lli", i.value->key);
            }
        }
        printf(" ]\n");
        const int fullSize = size_Map(map);
        int remCount = 0;
        for (int i = 0; i < 300; ++i) {
            iMapKey key = iRandom(0, 100);
            iMapNode *rem = remove_Map(map, key);
            if (rem) {
                remCount++;
                iAssert(rem->key == key);
            }
        }
        iAssert(fullSize - remCount == size_Map(map));
        printf("Size after removals: %zu\n", size_Map(map));
        delete_Map(map);
        iEndCollect();
    }
    /* Test tree nodes. */ {
        iTestNode *a = new_TestNode(1);
        iTestNode *b = new_TestNode(2);
        iSuperNode *c = new_SuperNode(3, 100);
        setParent_TreeNode(b, a);
        setParent_TreeNode(c, a);
        printf("Children:\n");
        iConstForEach(List, i, a->node.children) {
            printf("- %p\n", i.value);
        }
        delete_TreeNode(b);
        delete_TreeNode(a);
    }
    /* Test reference counting. */ {
        iTestObject *a = new_TestObject(123);
        iTestObject *b = ref_Object(a);
        printf("deref a...\n"); deref_Object(a);
        printf("deref b...\n"); deref_Object(b);
    }
    /* Test blocks and garbage collector. */ {
        iBeginCollect();
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
        iEndCollect();
    }
    /* Test Unicode strings. */ {
        iString *s = collect_String(newCStr_String("A_Äö\U0001f698a"));
        printf("String: %s length: %zu size: %zu\n", cstr_String(s), length_String(s), size_String(s)); {
            iConstForEach(String, i, s) {
                printf(" char: %04x [%lc]\n", i.value, i.value);
            }
        }
        printf("Backwards:\n"); {
            iReverseConstForEach(String, i , s) {
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
#if defined (iHavePcre)
    /* Test regular expressions. */ {
        iString *s = newCStr_String("Hello world Äöäö, there is a \U0001f698 out there.");
        iRegExp *rx = new_RegExp("\\b(THERE|WORLD|äöäö)\\b", caseInsensitive_RegExpOption);
        iRegExpMatch match;
        while (matchString_RegExp(rx, s, &match)) {
            iString *cap = captured_RegExpMatch(&match, 1);
            printf("match: %i -> %i [%s]\n", match.range.start, match.range.end, cstr_String(cap));
            delete_String(cap);
        }
        iRelease(rx);
        delete_String(s);
    }
#endif
#if defined (iHaveZlib)
    /* Test zlib compression. */ {
        iString *s = newCStr_String("Hello world. "
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
        printf("Restored %zu: %s\n", size_Block(restored), constData_Block(restored));
        delete_Block(restored);
        delete_Block(compr);
    }
#endif
    return 0;
}
