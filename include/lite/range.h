#pragma once

#include "defs.h"

#define lRange_size(self) \
    ((self)->end - (self)->start)        
#define lRange_isEmpty(self) \
    ((self)->end == (self)->start)
#define lRange_contains(self, value) \
    ((value) >= (self)->start && (value) < (self)->end)
#define lRange_shift(self, delta) \
    { (self)->start += (delta); (self)->end += (delta); }

struct lRangei_Impl {
    int start;
    int end;
};

struct lRangeui_Impl {
    uint start;
    uint end;    
};

struct lRanges_Impl {
    size_t start;
    size_t end;
};

LITE_DECLARE_IMPL(Rangei);
LITE_DECLARE_IMPL(Rangeui);
LITE_DECLARE_IMPL(Ranges);
