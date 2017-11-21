#include "lite/object.h"
#include "lite/array.h"
#include <stdio.h>

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

int main(int argc, char *argv[]) {
    /* Test list insertion and removal. */ {
        char c;
        iArray *list = iArray_new(2);
        printArray(list);

        iArray_pushBack(list, "00");
        printArray(list);

        iArray_pushBack(list, "11");
        printArray(list);

        iArray_pushBack(list, "22");
        printArray(list);

        iArray_pushBack(list, "33");
        printArray(list);

        iArray_pushBack(list, "44");
        printArray(list);

        iArray_pushBack(list, "55");
        printArray(list);

        iArray_pushBack(list, "66");
        printArray(list);

        iArray_pushBack(list, "77");
        printArray(list);

        iArray_pushBack(list, "88");
        printArray(list);

        iArray_pushBack(list, "99");
        printArray(list);

        iArray_insert(list, 7, "XX");
        printArray(list);

        iArray_insert(list, 7, "YY");
        printArray(list);

        iArray_insert(list, 8, "ZZ");
        printArray(list);

        iArray_pushFront(list, "aa");
        printArray(list);

        iArray_pushBack(list, "bb");
        printArray(list);

        iArray_pushBack(list, "cc");
        printArray(list);

        iArray_popBack(list);
        printArray(list);

        iArray_popBack(list);
        printArray(list);

        iArray_popBack(list);
        printArray(list);

        iArray_popBack(list);
        printArray(list);

        iArray_popBack(list);
        printArray(list);

        iArray_popBack(list);
        printArray(list);

        iArray_popFront(list);
        printArray(list);

        iArray_remove(list, 6);
        printArray(list);

        iArray_remove(list, 5);
        printArray(list);

        iArray_remove(list, 4);
        printArray(list);

        iArray_remove(list, 3);
        printArray(list);

        iArray_remove(list, 2);
        printArray(list);

        iArray_delete(list);
    }
    return 0;
}
