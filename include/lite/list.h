#pragma once

#include "lite/defs.h"

LITE_DECLARE_OPAQUE(List);
    
List *      List_New();
void        List_Delete(List *this);
