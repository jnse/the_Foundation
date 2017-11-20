#include "lite/object.h"
#include "lite/list.h"
#include <stdio.h>

void printList(const List *list) {
    printf("%4lu [", List_size(list));
    const char *ptr = List_data(list);
    for (int i = 0; i < List_size(list) * list->elementSize; ++i) {
        printf("%c", ptr[i]);
    }
    printf("]\n");
}

int main(int argc, char *argv[]) {
    // Object *obj = Object_New();
    // Object_Release(obj);
    {
        List *list = List_new(1);        
        printList(list);
        char c = 'A';
        List_pushBack(list, &c);
        printList(list);
        c = 'B';
        List_pushFront(list, &c);
        printList(list);
        List_delete(list);
    }
    return 0;
}
