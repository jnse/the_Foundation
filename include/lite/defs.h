#pragma once

#include <stddef.h>
#include <assert.h>

#define LITE_ASSERT(cond) assert(cond)

#define LITE_DECLARE_IMPL(className) \
    typedef struct l##className##_Impl l##className; \

// Types.
typedef unsigned int uint;
