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
    _mm_storeu_ps(v,      d->col[0]);
    _mm_storeu_ps(v + 4,  d->col[1]);
    _mm_storeu_ps(v + 8,  d->col[2]);
    _mm_storeu_ps(v + 12, d->col[3]);
}

void mul_Mat4(iMat4 *a, const iMat4 *b) /*, iMat4 *m_out)*/ {
    iMat4 result;
    //for (int i = 0; i < 16; i += 4) {
    for (int i = 0; i < 4; ++i) {
/*        iFloat4 rl = mul_F4(initv_F4(a->v), init1_F4(b->v[i]));
        for (int j = 1; j < 4; j++) {
            addv_F4(&rl, mul_F4(initv_F4(&a->v[j * 4]), init1_F4(b->v[i + j])));
        }*/
        const iFloat4 bCol = init128_F4(b->col[i]);
        __m128 rl = _mm_mul_ps(a->col[0], _mm_set1_ps(x_F4(bCol))); //x_F4(init128_F4(b->col[i/4]))));
        rl = _mm_add_ps(rl, _mm_mul_ps(a->col[1], _mm_set1_ps(y_F4(bCol))));
        rl = _mm_add_ps(rl, _mm_mul_ps(a->col[2], _mm_set1_ps(z_F4(bCol))));
        rl = _mm_add_ps(rl, _mm_mul_ps(a->col[3], _mm_set1_ps(w_F4(bCol))));

        //store_F4(rl, m_out->v + i);
        result.col[i] = rl;
    }
    copy_Mat4(a, &result);
}
