/** @file math_generic.c  Vector math, generic implementation.
 *
 * @authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>
 *
 * @par License
 * GPL: http://www.gnu.org/licenses/gpl.html
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details. You should have received a copy of the GNU
 * General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses</small>
 */

#include "t_fdn/math.h"

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
