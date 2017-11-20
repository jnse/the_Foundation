#pragma once

#include "lite/defs.h"

LITE_DECLARE_OPAQUE(Object);
#define SELF Object *

/**
 * Reference-counted object that owns child objects and may have a parent.
 * Objects get deleted only after all references have been removed.
 */

Object *    Object_new          (void);
void        Object_release      (SELF);

void        Object_setParent    (SELF, Object *parent);

Object *    Object_parent       (const SELF);

#undef SELF
