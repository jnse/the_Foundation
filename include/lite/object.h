#pragma once

#include "lite/defs.h"

LITE_DECLARE_IMPL(Object);

/**
 * Reference-counted object that owns child objects and may have a parent.
 * Objects get deleted only after all references have been removed.
 */

iObject *   iObject_new         (void);
void        iObject_release     (iObject *);

void        iObject_setParent   (iObject *, iObject *parent);

iObject *   iObject_parent      (const iObject *);
