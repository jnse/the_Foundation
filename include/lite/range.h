#pragma once

#include "defs.h"

#define Range_Size(self) \
    ((self)->end - (self)->start)        
#define Range_IsEmpty(self) \
    ((self)->end == (self)->start)
#define Range_Contains(self, value) \
    ((value) >= (self)->start && (value) < (self)->end)
#define Range_Shift(self, delta) \
    { (self)->start += (delta); (self)->end += (delta); }

struct Rangei_Impl {
    int start;
    int end;
};
LITE_DECLARE_OPAQUE(Rangei);

struct Rangeui_Impl {
    uint start;
    uint end;    
};
LITE_DECLARE_OPAQUE(Rangeui);

struct Ranges_Impl {
    size_t start;
    size_t end;
};
LITE_DECLARE_OPAQUE(Ranges);
