#pragma once

#include "lite/defs.h"

LITE_DECLARE_OPAQUE(Object);
#define SELF Object *

/**
 * Reference-counted object that owns child objects and may have a parent.
 * Objects get deleted only after all references have been removed.
 */

Object *    Object_New          (void);
void        Object_Release      (SELF);

void        Object_SetParent    (SELF, Object *parent);

Object *    Object_Parent       (const SELF);

#undef SELF
