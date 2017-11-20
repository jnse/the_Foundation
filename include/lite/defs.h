#pragma once

#include <stddef.h>
#include <assert.h>

#define LITE_ASSERT(cond) assert(cond)

#define LITE_DECLARE_OPAQUE(className) \
    typedef struct className##_Impl className; \

// Types.
typedef unsigned int uint;
