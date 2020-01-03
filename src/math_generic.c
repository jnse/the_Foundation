/** @file math_generic.c  Vector math, generic implementation.

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

#include "the_Foundation/math.h"

void init_Mat4(iMat4 *d) {
    d->col[0] = init_F4(1, 0, 0, 0);
    d->col[1] = init_F4(0, 1, 0, 0);
    d->col[2] = init_F4(0, 0, 1, 0);
    d->col[3] = init_F4(0, 0, 0, 1);
}

void store_Mat4(const iMat4 *d, float *v) {
    store_F4(d->col[0], v);
    store_F4(d->col[1], v + 4);
    store_F4(d->col[2], v + 8);
    store_F4(d->col[3], v + 12);
}

void load_Mat4(iMat4 *d, const float *v) {
    d->col[0] = initv_F4(v);
    d->col[1] = initv_F4(v + 4);
    d->col[2] = initv_F4(v + 8);
    d->col[3] = initv_F4(v + 12);
}

void mul_Mat4(iMat4 *d, const iMat4 *a) {
    iMat4 result;
    for (int i = 0; i < 4; ++i) {
        const iFloat4 dCol = d->col[i];
        iFloat4 rl = mul_F4(a->col[0], init1_F4(x_F4(dCol)));
        addv_F4(&rl, mul_F4(a->col[1], init1_F4(y_F4(dCol))));
        addv_F4(&rl, mul_F4(a->col[2], init1_F4(z_F4(dCol))));
        addv_F4(&rl, mul_F4(a->col[3], init1_F4(w_F4(dCol))));
        result.col[i] = rl;
    }
    copy_Mat4(d, &result);
}

void initRotate_Mat4(iMat4 *d, iFloat3 axis, float degrees) {
    const float ang = iMathDegreeToRadianf(degrees);
    const float c   = cosf(ang);
    const float s   = sinf(ang);
    const iFloat3 normAxis = normalize_F3(axis);
    const float *ax = normAxis.v;
    const iFloat4 axis4 = { .v = { normAxis.value.x, normAxis.value.y, normAxis.value.z, 0.f } };
    for (int i = 0; i < 3; ++i) {
        d->col[i] = mul_F4(mul_F4(axis4, init1_F4(ax[i])), init1_F4(1 - c));
    }
    addv_F4(&d->col[0], init_F4(+c,       +ax[2]*s,   -ax[1]*s,   0));
    addv_F4(&d->col[1], init_F4(-ax[2]*s, +c,         +ax[0]*s,   0));
    addv_F4(&d->col[2], init_F4(+ax[1]*s, -ax[0]*s,   +c,         0));
    d->col[3] = init_F4(0, 0, 0, 1);
}

void store_Mat3(const iMat3 *d, float *v9) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            v9[3*i + j] = d->col[i].v[j];
        }
    }
}
