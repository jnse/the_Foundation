#pragma once

typedef struct Object_Impl Object;

struct Object_Class {
    Object *(*new)(int value);
    void (*delete)(Object *this);
    int (*member)(const Object *this);
};

extern struct Object_Class CObject;

Object *Object_New(int value);
void Object_Delete(Object *this);
int Object_Member(const Object *this);
