#include "object.h"

#include <stdio.h>
#include <stdlib.h>

struct Object_Impl {
    int member;
};

Object *Object_New(int value) {
    Object *this = malloc(sizeof(Object));
    this->member = value;
    printf("constructed Object %p\n", this);
    return this;
}

void Object_Delete(Object *this) {
    printf("deleting Object %p\n", this);
    free(this);
}

int Object_Member(const Object *this) {
    return this->member;
}

struct Object_Class CObject = {
    .new = Object_New,
    .delete = Object_Delete,
    .member = Object_Member
};
