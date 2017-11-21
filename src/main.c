#include "lite/object.h"
#include "lite/array.h"
#include <stdio.h>

void printArray(const lArray *list) {
    printf("%4lu [", lArray_size(list));
    const char *ptr = lArray_data(list);
    for (int i = 0; i < lArray_size(list) * list->elementSize; ++i) {
        printf("%c", ptr[i]);
    }
    printf("]\n");
}

int main(int argc, char *argv[]) {
    /* Test some lists. */ {
        char c;
        lArray *list = lArray_new(1);                
        printArray(list);
        
        lArray_pushBack(list, "A");
        printArray(list);
        
        lArray_pushFront(list, "B");
        printArray(list);
        
        lArray_delete(list);
    }
    return 0;
}
