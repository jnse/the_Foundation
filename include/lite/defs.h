#pragma once

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define LITE_ASSERT(cond) assert(cond)

#define LITE_DECLARE_IMPL(className) \
    typedef struct i_##className##_Impl i##className

#define iFalse  0
#define iTrue   1

// Types.
typedef int             iBool;
typedef uint8_t         iByte;
typedef uint16_t        iChar16;
typedef unsigned int    iUInt;
