#pragma once

#include "defs.h"

#define iRange_size(self) \
    ((self)->end - (self)->start)
#define iRange_isEmpty(self) \
    ((self)->end == (self)->start)
#define iRange_contains(self, value) \
    ((value) >= (self)->start && (value) < (self)->end)
#define iRange_shift(self, delta) \
    { (self)->start += (delta); (self)->end += (delta); }

struct i_Rangei_Impl {
    int start;
    int end;
};

struct i_Rangeui_Impl {
    uint start;
    uint end;
};

struct i_Ranges_Impl {
    size_t start;
    size_t end;
};

LITE_DECLARE_IMPL(Rangei);
LITE_DECLARE_IMPL(Rangeui);
LITE_DECLARE_IMPL(Ranges);
