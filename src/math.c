/** @file math.c  Math routines.

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

#include "c_plus/math.h"

#include <stdlib.h>

int iRandom(int start, int end) {
    return start + rand() % (end - start);
}

float iRandomf(void) {
    return rand() / (float) RAND_MAX;
}

//---------------------------------------------------------------------------------------

static float determinant_Mat3_(const float *mat) {
    return (mat[0] * ( mat[4] * mat[8] - mat[7] * mat[5] ) -
            mat[1] * ( mat[3] * mat[8] - mat[6] * mat[5] ) +
            mat[2] * ( mat[3] * mat[7] - mat[6] * mat[4] ));
}

static iBool inverse9_Mat3_(const float *mat, float *inverse_out) {
    const float det = determinant_Mat3_(mat);
    if (fabsf(det) < .000001f) {
        iMat3 identity;
        init_Mat3(&identity);
        store_Mat3(&identity, inverse_out);
        return iFalse;
    }
    inverse_out[0] =    mat[4] * mat[8] - mat[5] * mat[7]   / det;
    inverse_out[1] = -( mat[1] * mat[8] - mat[7] * mat[2] ) / det;
    inverse_out[2] =    mat[1] * mat[5] - mat[4] * mat[2]   / det;
    inverse_out[3] = -( mat[3] * mat[8] - mat[5] * mat[6] ) / det;
    inverse_out[4] =    mat[0] * mat[8] - mat[6] * mat[2]   / det;
    inverse_out[5] = -( mat[0] * mat[5] - mat[3] * mat[2] ) / det;
    inverse_out[6] =    mat[3] * mat[7] - mat[6] * mat[4]   / det;
    inverse_out[7] = -( mat[0] * mat[7] - mat[6] * mat[1] ) / det;
    inverse_out[8] =    mat[0] * mat[4] - mat[1] * mat[3]   / det;
    return iTrue;
}

iBool inverse_Mat3(const iMat3 *d, iMat3 *inversed_out) {
    float result[9];
    float d9[9];
    store_Mat3(d, d9);
    const iBool ok = inverse9_Mat3_(d9, result);
    load_Mat3(inversed_out, result);
    return ok;
}

static void submatrix_Mat4(const float *mat4, float *mat3, int i, int j) {
    // Loop through 3x3 submatrix.
    for (int di = 0; di < 3; di++) {
        for (int dj = 0; dj < 3; dj++) {
            // Map 3x3 element (destination) to 4x4 element (source).
            int si = di + (di >= i? 1 : 0);
            int sj = dj + (dj >= j? 1 : 0);
            // Copy element.
            mat3[di * 3 + dj] = mat4[si * 4 + sj];
        }
    }
}

static float determinant_Mat4(const float *mat) {
    float result = 0;
    float i = 1;
    for (int n = 0; n < 4; n++, i *= -1) {
        float sub[3*3];
        submatrix_Mat4(mat, sub, 0, n);
        result += mat[n] * determinant_Mat3_(sub) * i;
    }
    return result;
}

static iBool inverse16_Mat4_(const float *in16, float *out16) {
    const float det = determinant_Mat4(in16);
    if (fabsf(det) < .000001f) {
        iMat4 identity;
        init_Mat4(&identity);
        store_Mat4(&identity, out16);
        return iFalse;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sub[3*3];
            const float sign = (float) (1 - ((i + j) % 2) * 2);
            submatrix_Mat4(in16, sub, i, j);
            out16[i + j*4] = (determinant_Mat3_(sub) * sign) / det;
        }
    }
    return iTrue;
}

iBool inverse_Mat4(const iMat4 *d, iMat4 *inversed_out) {
    float result[16];
    float d16[16];
    store_Mat4(d, d16);
    const iBool ok = inverse16_Mat4_(d16, result);
    if (ok) load_Mat4(inversed_out, result);
    return ok;
}
