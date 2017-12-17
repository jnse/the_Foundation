/** @file math_sse.c  Vector math using SSE.
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

#include "c_plus/math.h"

void init_Mat4(iMat4 *d) {
    d->col[0] = init_F4(1, 0, 0, 0).m;
    d->col[1] = init_F4(0, 1, 0, 0).m;
    d->col[2] = init_F4(0, 0, 1, 0).m;
    d->col[3] = init_F4(0, 0, 0, 1).m;
}

void store_Mat4(const iMat4 *d, float *v) {
    _Alignas(16) float f[16];
    _mm_store_ps(f,      d->col[0]);
    _mm_store_ps(f + 4,  d->col[1]);
    _mm_store_ps(f + 8,  d->col[2]);
    _mm_store_ps(f + 12, d->col[3]);
    for (int i = 0; i < 16; i += 4) {
        v[i + 0] = f[i + 1];
        v[i + 1] = f[i + 2];
        v[i + 2] = f[i + 3];
        v[i + 3] = f[i + 0];
    }
}

void mul_Mat4(iMat4 *d, const iMat4 *other) {
    iMat4 result;
    for (int i = 0; i < 4; ++i) {
        const iFloat4 otherCol = initmm_F4(other->col[i]);
        __m128 rl =         _mm_mul_ps(d->col[0], _mm_set1_ps(x_F4(otherCol)));
        rl = _mm_add_ps(rl, _mm_mul_ps(d->col[1], _mm_set1_ps(y_F4(otherCol))));
        rl = _mm_add_ps(rl, _mm_mul_ps(d->col[2], _mm_set1_ps(z_F4(otherCol))));
        rl = _mm_add_ps(rl, _mm_mul_ps(d->col[3], _mm_set1_ps(w_F4(otherCol))));
        result.col[i] = rl;
    }
    copy_Mat4(d, &result);
}
