#pragma once

#include <stddef.h>
#include <assert.h>

#define LITE_ASSERT(cond) assert(cond)

#define LITE_DECLARE_IMPL(className) \
    typedef struct i_##className##_Impl i##className

// Types.
typedef unsigned int uint;
