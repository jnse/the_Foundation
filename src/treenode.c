/** @file treenode.c  Tree node base class.

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

#include "the_Foundation/treenode.h"
#include "the_Foundation/ptrarray.h"
#include "the_Foundation/ptrset.h"

#include <stdio.h>
#include <stdlib.h>

static iList *children_TreeNode_(iTreeNode *d) {
    if (!d->children) {
        d->children = new_List();
    }
    return d->children;
}

void *new_TreeNode(const iAnyClass *class) {
    iAssert(class != NULL);
    iAssert(((const iClass *) class)->size >= sizeof(iTreeNode));
    iTreeNode *d = malloc(((const iClass *) class)->size);
    iZap(d->node);
    d->class = (const iClass *) class;
    d->parent = NULL;
    d->children = NULL;
    printf("new Object %p\n", d);
    return d;
}

void delete_TreeNode(iAnyTreeNode *any) {
    iTreeNode *d = (iTreeNode *) any;
    setParent_TreeNode(d, NULL);
    /* Destroy children, who will remove themselves. */
    while (!isEmpty_List(d->children)) {
        delete_TreeNode(front_List(d->children));
    }
    delete_List(d->children);
    deinit_Class(d->class, d);
//    printf("deleted Object %p\n", d);
    free(d);
}

void setParent_TreeNode(iAnyTreeNode *any, iAnyTreeNode *parent) {
    iTreeNode *d = (iTreeNode *) any;
    if (d->parent == parent) return;
    if (d->parent) {
        /* Remove from old parent. */
        iAssert(d->parent->children);
        remove_List(d->parent->children, d);
    }
    d->parent = parent;
    if (parent) {
        pushBack_List(children_TreeNode_(d->parent), d);
    }
}

iAnyTreeNode *parent_TreeNode(const iAnyTreeNode *d) {
    return ((const iTreeNode *) d)->parent;
}

const iList *children_TreeNode(const iAnyTreeNode *d) {
    return children_TreeNode_(iConstCast(iTreeNode *, d));
}
