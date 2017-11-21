#pragma once

#include "lite/defs.h"

LITE_DECLARE_IMPL(Object);

/**
 * Reference-counted object that owns child objects and may have a parent.
 * Objects get deleted only after all references have been removed.
 */

lObject *   lObject_new         (void);
void        lObject_release     (lObject *);

void        lObject_setParent   (lObject *, lObject *parent);

lObject *   lObject_parent      (const lObject *);
