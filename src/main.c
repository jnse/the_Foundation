#include <stdio.h>
#include "object.h"

int main(int argc, char *argv[]) {
    Object *obj = Object_New(12345);    
    printf("Simple code is great: %i\n", CObject.member(obj));
    CObject.delete(obj);
    return 0;
}
