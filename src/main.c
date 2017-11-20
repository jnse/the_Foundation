#include "lite/object.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    Object *obj = Object_New();    
    Object_Release(obj);
    return 0;
}
