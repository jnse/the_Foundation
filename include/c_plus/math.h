#pragma once

/** @file c_plus/math.h  Math routines and constants.

@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>

@par License

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

<small>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</small>
*/

#include "defs.h"

iBeginPublic

#define iMathPi                 3.14159265358979323846
#define iMathPif                3.14159265358979323846f
#define iMathDegreeToRadianf(v) ((v) * iMathPif / 180.f)
#define iMathRadianToDegreef(v) ((v) * 180.f / iMathPif)

int     iRandom(int start, int end);
float   iRandomf(void);

iDeclareType(FloatVec3)
iDeclareType(FloatVec4)

struct Impl_FloatVec3 {
    union {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        } value;
    };
};

struct Impl_FloatVec4 {
    union {
        float v[4];
        struct {
            float x;
            float y;
            float z;
            float w;
        } value;
    };
};

#if defined (iHaveSSE4_1)
#   include "math_sse.h"
#else
#   include "math_generic.h"
#endif

iBool inverse_Mat3(const iMat3 *d, iMat3 *inversed_out);
iBool inverse_Mat4(const iMat4 *d, iMat4 *inversed_out);

iEndPublic
