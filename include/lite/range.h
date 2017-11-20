#pragma once

#include "defs.h"

#define Range_size(self) \
    ((self)->end - (self)->start)        
#define Range_isEmpty(self) \
    ((self)->end == (self)->start)
#define Range_contains(self, value) \
    ((value) >= (self)->start && (value) < (self)->end)
#define Range_shift(self, delta) \
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
