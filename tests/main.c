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
#include <c_plus/commandline.h>
#include <c_plus/file.h>
#include <c_plus/fileinfo.h>
#include <c_plus/garbage.h>
#include <c_plus/hash.h>
#include <c_plus/map.h>
#include <c_plus/math.h>
#include <c_plus/hash.h>
#include <c_plus/object.h>
#include <c_plus/objectlist.h>
#include <c_plus/path.h>
#include <c_plus/ptrarray.h>
#include <c_plus/regexp.h>
#include <c_plus/sortedarray.h>
#include <c_plus/string.h>
#include <c_plus/stringarray.h>
#include <c_plus/stringlist.h>
#include <c_plus/stringhash.h>
#include <c_plus/time.h>
#include <c_plus/thread.h>
#include <c_plus/treenode.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

//---------------------------------------------------------------------------------------

iDeclareStaticClass(TestNode)
iDeclareType(TestNode)

struct Impl_TestNode {
    iTreeNode node;
    int value;
};

void init_TestNode(iTestNode *d, int value) {
    d->value = value;
}

void deinit_TestNode(iAny *any) {
    iDebug("deinit TestNode: %i\n", ((iTestNode *) any)->value);
}

static iDefineClass(TestNode)

iTestNode *new_TestNode(int value) {
    iTestNode *d = new_TreeNode(&Class_TestNode);
    init_TestNode(d, value);
    return d;
}

//---------------------------------------------------------------------------------------

iDeclareStaticClass(SuperNode)
iDeclareType(SuperNode)

struct Impl_SuperNode {
    iTestNode testNode;
    int member;
};

void init_SuperNode(iSuperNode *d, int member) {
    d->member = member;
}

void deinit_SuperNode(iAny *any) {
    iDebug("deinit SuperNode: %i\n", ((iSuperNode *) any)->member);
}

static iDefineSubclass(SuperNode, TestNode)

iSuperNode *new_SuperNode(int value, int member) {
    iSuperNode *d = new_TreeNode(&Class_SuperNode);
    init_TestNode(&d->testNode, value);
    init_SuperNode(d, member);
    return d;
}

//---------------------------------------------------------------------------------------

iDeclareStaticClass(TestObject)
iDeclareType(TestObject)

struct Impl_TestObject {
    iObject object;
    int value;
};

static void deinit_TestObject(iAnyObject *any) {
    iDebug("deinit TestObject: %i\n", ((iTestObject *) any)->value);
}

static iDefineClass(TestObject)

iTestObject *new_TestObject(int value) {
    iTestObject *d = iNew(TestObject);
    d->value = value;
    return d;
}

//---------------------------------------------------------------------------------------

void printArray(const iArray *d) {
    printf("%4lu %4lu -> %-4lu : %4lu [", size_Array(d), d->range.start, d->range.end, d->allocSize);
    for (size_t i = 0; i < d->allocSize * d->elementSize; ++i) {
        if (i/d->elementSize < d->range.start || i/d->elementSize >= d->range.end)
            printf(" _");
        else
            printf(" %c", d->data[i]);
    }
    puts(" ]");
}

static void printIntArray(const iArray *d) {
    printf("%4lu :", size_Array(d));
    iConstForEach(Array, i, d) {
        printf(" %2i", *(const int *) i.value);
    }
    puts("");
}

static void printIntPairArray(const iArray *d) {
    printf("%4lu :", size_Array(d));
    iConstForEach(Array, i, d) {
        const int *pair = (const int *) i.value;
        printf(" (%2i, %2i)", pair[0], pair[1]);
    }
    puts("");
}

static int compareTextElements(const void *a, const void *b) {
    return iCmpStrN(a, b, 2);
}

static int compareIntElements(const void *a, const void *b) {
    return iCmp(*(const int *) a, *(const int *) b);
}

static int compareIntPairElements(const void *a, const void *b) {
    const int cmp = compareIntElements(a, b);
    if (cmp != 0) return cmp;
    const int *x = a, *y = b;
    return iCmp(x[1], y[1]);
}

static int compareIntegers(iMapKey a, iMapKey b) {
    return iCmp(a, b);
}

static int compareTestObjects(const void *a, const void *b) {
    const iTestObject *x = object_ObjectListNode(a);
    const iTestObject *y = object_ObjectListNode(b);
    iAssert(a != b);
    //iDebug("comparing: (%p) %i and (%p) %i\n", x, x->value, y, y->value);
    return iCmp(x->value, y->value);
}

static iThreadResult run_WorkerThread(iThread *d) {
    printf("Worker thread %p started\n", d);
    printf("Ideal concurrent thread count: %i\n", idealConcurrentCount_Thread());
    sleep_Thread(0.1);
    printf("Worker thread %p is done\n", d);
    return 12345;
}

int main(int argc, char *argv[]) {
    init_CPlus();
    /* Test command line options parsing. */ {
        iCommandLine *cmdline = iClob(new_CommandLine(argc, argv));
        defineValues_CommandLine(cmdline, "file;f", unlimitedValues_CommandLine);
        defineValues_CommandLine(cmdline, "value;V", 1);
        printf("Executable: \"%s\"\n", cstr_String(executablePath_CommandLine(cmdline)));
        printf("Working directory: \"%s\"\n", cstr_String(collect_String(cwd_Path())));
        puts("Options from command line:");
        iConstForEach(StringList, i, args_CommandLine(cmdline)) {
            printf("%2zu: \"%s\"\n", i.pos, cstr_String(i.value));
        }
        puts("Iterated:");
        iConstForEach(CommandLine, j, cmdline) {
            iString *e = newRange_String(&j.entry);
            printf("[%2zu] %5s vc:%zu \"%s\"\n",
                   j.value,
                   ( j.argType == value_CommandLineArgType? "value"
                   : j.argType == shortArgument_CommandLineArgType? "short" : "long" ),
                   j.valueCount,
                   cstr_String(e));
            const iCommandLineArg *arg = argument_CommandLineConstIterator(&j);
            if (arg && !isEmpty_StringList(&arg->values)) {
                printf("  arguments for \"%s\":\n", cstr_String(&arg->arg));
                iConstForEach(StringList, i, &arg->values) {
                    printf("    %2zu: \"%s\"\n", i.pos, cstr_String(i.value));
                }
                iRelease(arg);
            }
            delete_String(e);
        }
        iCommandLineArg *arg;
        arg = iClob(checkArgument_CommandLine(cmdline, "file"));
        if (arg) {
            printf("file option:");
            iConstForEach(StringList, k, values_CommandLineArg(arg)) {
                iString *clean = copy_String(k.value);
                clean_Path(clean);
                printf(" %s", cstr_String(clean));
                delete_String(clean);
            }
            puts("");
        }
        arg = iClob(checkArgumentValues_CommandLine(cmdline, "V;value", 1));
        if (arg) {
            printf("value option: %s\n", cstr_String(value_CommandLineArg(arg, 0)));
        }
        if (contains_CommandLine(cmdline, "x")) {
            puts("x option");
        }
        if (contains_CommandLine(cmdline, "f")) {
            puts("f option");
        }
        if (contains_CommandLine(cmdline, "d")) {
            puts("d option");
        }
    }
    return -100;
    /* Test time and date. */ {
        const iTime now = now_Time();
        iDate date;
        init_Date(&date, &now);
        printf("sizeof(iTime) == %zu bytes\n", sizeof(iTime));
        printf("sizeof(iDate) == %zu bytes\n", sizeof(iDate));
        printf("Today is %i-%02i-%02i (week day %i) and the time is %02i:%02i:%02i.%li (GMT offset: %li mins)\n",
               date.year, date.month, date.day, date.dayOfWeek,
               date.hour, date.minute, date.second, date.nsecs,
               date.gmtOffsetSeconds/60);
    }
    /* File information. */ {
        iBeginCollect();
        iForEach(DirFileInfo, i, iClob(newCStr_DirFileInfo("."))) {
            printf("%10li %s %12li %s\n",
                   size_FileInfo(i.value),
                   isDirectory_FileInfo(i.value)? "(dir)" : "     ",
                   lastModified_FileInfo(i.value).ts.tv_sec,
                   cstrPath_FileInfo(i.value));
        }
        iEndCollect();
    }
    /* Test array insertion and removal. */ {
        puts("Array insertions/removals:");
        iArray *list = new_Array(2);
        printArray(list);
        {
            puts("Iterating the empty array:");
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
            puts("Iterating the array:");
            iConstForEach(Array, i, list) {
                printf("- %p\n", i.value);
            }
        }
        delete_Array(list);
    }
    /* Test a sorted array. */ {
        iSortedArray *ints = new_SortedArray(sizeof(int), compareIntElements);
        puts("Sorted array of integers:");
        for (int i = 0; i < 6; ++i) {
            insert_SortedArray(ints, &(int){ iRandom(0, 10) });
        }
        printIntArray(&ints->values);
        delete_SortedArray(ints);

        iSortedArray *pairs = new_SortedArray(sizeof(int[2]), compareIntPairElements);
        puts("Sorted array of integer pairs:");
        for (int i = 0; i < 6; ++i) {
            insert_SortedArray(pairs, &(int[2]){ iRandom(0, 3), iRandom(0, 10) });
        }
        printIntPairArray(&pairs->values);
        const iRanges ones = locateRange_SortedArray(pairs, &(int[2]){ 1, 0 },
                compareIntElements);
        printf("Major 1s are located at [%zu, %zu)\n", ones.start, ones.end);
        delete_SortedArray(pairs);
    }
    /* Test an array of pointers. */ {
        iPtrArray *par = newPointers_PtrArray("Entry One", "Entry Two", NULL);
        puts("Iterating the pointer array:");
        iConstForEach(PtrArray, i, par) {
            printf("- %s\n", i.ptr);
        }
        delete_PtrArray(par);
    }
    /* Test an object list. */ {
        iObjectList *olist = new_ObjectList();
        for (int i = 0; i < 10; ++i) {
            pushBack_ObjectList(olist, iClob(new_TestObject(iRandom(1, 1000))));
        }
        sort_List(list_ObjectList(olist), compareTestObjects);
        printf("List of objects:");
        iConstForEach(ObjectList, i, olist) {
            printf("%4i", ((const iTestObject *) i.object)->value);
        }
        puts("");
        iRelease(olist);
    }
    /* Test a string hash. */ {
        iStringHash *h = new_StringHash();
        insertValuesCStr_StringHash(h,
              "one", iClob(new_TestObject(1000)),
              "two", iClob(new_TestObject(1001)), NULL);
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
            iHashNode *node = iCollectMem(iMalloc(HashNode));
            node->key = i;
            insert_Hash(h, node);
        }
        puts("Hash iteration:");
        int counter = 0;
        iForEach(Hash, i, h) {
            printf("%4i: %i\n", counter++, i.value->key);
        }
        delete_Hash(h);
    }
    /* Test a map. */ {
        iBeginCollect();
        puts("Testing a map.");
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
                iCollectMem(i.value);
            }
        }
        puts(" ]");
        printf("Keys in reverse: ["); {
            iReverseConstForEach(Map, i, map) {
                printf(" %2lli", i.value->key);
            }
        }
        puts(" ]");
        const int fullSize = size_Map(map);
        iDebugOnly(fullSize);
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
        puts("Children:");
        iConstForEach(List, i, a->node.children) {
            printf("- %p\n", i.value);
        }
        delete_TreeNode(b);
        delete_TreeNode(a);
    }
    /* Test reference counting. */ {
        iTestObject *a = new_TestObject(123);
        iTestObject *b = ref_Object(a);
        puts("deref a..."); deref_Object(a);
        puts("deref b..."); deref_Object(b);
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
    /* Test a thread. */ {
        iThread *worker = new_Thread(run_WorkerThread);
        start_Thread(worker);
        printf("Result from worker: %li\n", result_Thread(worker));
        iAssert(result_Thread(worker) == 12345);
        iRelease(worker);
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
