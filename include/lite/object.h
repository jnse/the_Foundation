#pragma once

#include "lite/defs.h"

LITE_DECLARE_OPAQUE(Object);

/**
 * Reference-counted object that owns child objects and may have a parent.
 * Objects get deleted only after all references have been removed.
 */

Object *    Object_New          (void);
void        Object_Release      (Object *this);

void        Object_SetParent    (Object *this, Object *parent);

Object *    Object_Parent       (const Object *this);
