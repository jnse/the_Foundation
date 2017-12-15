#pragma once

/** @file math_sse.h  Vector math using SSE.
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

#include "defs.h"
#include "math.h"

#include <math.h>
#include <xmmintrin.h>

iDeclareType(Float4)
iDeclareType(FloatVec3)
iDeclareType(FloatVec4)

struct Impl_Float4 {
    __m128 m;
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

static inline iFloat4 zero_Float4(void) {
    return (iFloat4){ _mm_setzero_ps() };
}

static inline iFloat4 init_Float4(float x, float y, float z, float w) {
    return (iFloat4){ _mm_set_ps(w, z, y, x) };
}

static inline iFloat4 initi_Float4(int x, int y, int z, int w) {
    return init_Float4((float) x, (float) y, (float) z, (float) w);
}

static inline iFloat4 initv_Float4(const float *v) {
    return (iFloat4){ _mm_set_ps(v[3], v[2], v[1], v[0]) };
}

static inline iFloat4 init128_Float4(__m128 m) {
    return (iFloat4){ m };
}

static inline float x_Float4(const iFloat4 d) {
    return _mm_cvtss_f32(d.m);
}

static inline float y_Float4(const iFloat4 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(1, 1, 1, 1)));
}

static inline float z_Float4(const iFloat4 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(2, 2, 2, 2)));
}

static inline float w_Float4(const iFloat4 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(3, 3, 3, 3)));
}

static inline iFloatVec4 values_Float4(const iFloat4 d) {
    return (iFloatVec4){ .value = { x_Float4(d), y_Float4(d), z_Float4(d), w_Float4(d) } };
}

static inline void load_Float4(const iFloat4 d, float *p_out) {
    p_out[0] = x_Float4(d);
    p_out[1] = y_Float4(d);
    p_out[2] = z_Float4(d);
    p_out[3] = w_Float4(d);
}

#define iFloatShuffle3(d, X, Y, Z) (iFloat4){ _mm_shuffle_ps((d).m, (d).m, _MM_SHUFFLE(3, Z, Y, X)) }

static inline iFloat4 yzx_Float4(const iFloat4 d) {
    return iFloatShuffle3(d, 1, 2, 0);
}

static inline iFloat4 zxy_Float4(const iFloat4 d) {
    return iFloatShuffle3(d, 2, 0, 1);
}

static inline iFloat4 xzy_Float4(const iFloat4 d) {
    return iFloatShuffle3(d, 0, 2, 1);
}

static inline void setX_Float4(iFloat4 *d, float x) {
    d->m = _mm_move_ss(d->m, _mm_set_ss(x));
}

static inline void setY_Float4(iFloat4 *d, float y) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(y));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setZ_Float4(iFloat4 *d, float z) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(z));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setW_Float4(iFloat4 *d, float w) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(w));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

typedef iFloat4 iBool4;

static inline iFloat4 add_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_add_ps(a.m, b.m) }; }
static inline iFloat4 sub_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_sub_ps(a.m, b.m) }; }
static inline iFloat4 mul_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_mul_ps(a.m, b.m) }; }
static inline iFloat4 mulf_Float4   (const iFloat4 a, const float b)    { return (iFloat4){ _mm_mul_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat4 fmul_Float4   (const float a, const iFloat4 b)    { return (iFloat4){ _mm_mul_ps(_mm_set1_ps(a), b.m) }; }
static inline iFloat4 div_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_div_ps(a.m, b.m) }; }
static inline iFloat4 divf_Float4   (const iFloat4 a, const float b)    { return (iFloat4){ _mm_div_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat4 fdiv_Float4   (const float a, const iFloat4 b)    { return (iFloat4){ _mm_div_ps(_mm_set1_ps(a), b.m) }; }

static inline iFloat4 addv_Float4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_add_ps(a->m, b.m); return *a; }
static inline iFloat4 subv_Float4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_sub_ps(a->m, b.m); return *a; }
static inline iFloat4 mulv_Float4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_mul_ps(a->m, b.m); return *a; }
static inline iFloat4 mulvf_Float4  (iFloat4 *a, const float b)         { a->m = _mm_mul_ps(a->m, _mm_set1_ps(b)); return *a; }
static inline iFloat4 divv_Float4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_div_ps(a->m, b.m); return *a; }
static inline iFloat4 divvf_Float4  (iFloat4 *a, const float b)         { a->m = _mm_div_ps(a->m, _mm_set1_ps(b)); return *a; }

static inline iBool4 equal_Float4   (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpeq_ps(a.m, b.m) }; }
static inline iBool4 notEqual_Float4(const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpneq_ps(a.m, b.m) }; }
static inline iBool4 less_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmplt_ps(a.m, b.m) }; }
static inline iBool4 greater_Float4 (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpgt_ps(a.m, b.m) }; }
static inline iBool4 lessEqual_Float4   (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmple_ps(a.m, b.m) }; }
static inline iBool4 greaterEqual_Float4(const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpge_ps(a.m, b.m) }; }

static inline iFloat4 min_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_min_ps(a.m, b.m) }; }
static inline iFloat4 max_Float4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_max_ps(a.m, b.m) }; }

static inline iFloat4 neg_Float4    (const iFloat4 a)                   { return (iFloat4){ _mm_sub_ps(_mm_setzero_ps(), a.m) }; }

#define iFloat4SignBits ((union { __m128 m; uint32_t m32[4]; }){ .m32 = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 } }).m

static inline iFloat4 abs_Float4(const iFloat4 a) {
    return (iFloat4){ _mm_andnot_ps(iFloat4SignBits, a.m) };
}

static inline unsigned mask_Float4  (const iFloat4 a)   { return _mm_movemask_ps(a.m) & 15; }
static inline iBool any_Bool4       (const iBool4 a)    { return mask_Float4(a) != 0; }
static inline iBool all_Bool4       (const iBool4 a)    { return mask_Float4(a) == 15; }

static inline iFloat4 clamp_Float4  (const iFloat4 t, const iFloat4 a, const iFloat4 b) { return min_Float4(max_Float4(t, a), b); }
static inline float sum_Float4      (const iFloat4 a)   { return x_Float4(a) + y_Float4(a) + z_Float4(a) + w_Float4(a); }
static inline float dot_Float4      (const iFloat4 a, const iFloat4 b) { return sum_Float4(mul_Float4(a, b)); }
static inline float lengthSq_Float4 (const iFloat4 a)   { return dot_Float4(a, a); }
static inline float length_Float4   (const iFloat4 a)   { return sqrtf(lengthSq_Float4(a)); }
static inline iFloat4 normalize_Float4(const iFloat4 a) { return mulf_Float4(a, 1.f / length_Float4(a)); }

static inline iFloat4 mix_Float4   (const iFloat4 a, const iFloat4 b, float t) {
    return add_Float4(a, mulf_Float4(sub_Float4(b, a), t));
}

//---------------------------------------------------------------------------------------

typedef iFloat4 iFloat3;

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

static inline iFloat3 zero_Float3(void) {
    return (iFloat3){ _mm_setzero_ps() };
}

static inline iFloat3 init_Float3(float x, float y, float z) {
    return (iFloat3){ _mm_set_ps(z, z, y, x) };
}

static inline iFloat3 initi_Float3(int x, int y, int z) {
    return init_Float3((float) x, (float) y, (float) z);
}

static inline iFloat3 initv_Float3(const float *v) {
    return (iFloat3){ _mm_set_ps(v[2], v[2], v[1], v[0]) };
}

static inline float x_Float3(const iFloat3 d) {
    return _mm_cvtss_f32(d.m);
}

static inline float y_Float3(const iFloat3 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(1, 1, 1, 1)));
}

static inline float z_Float3(const iFloat3 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(2, 2, 2, 2)));
}

static inline iFloatVec3 values_Float3(const iFloat3 d) {
    return (iFloatVec3){ .value = { x_Float3(d), y_Float3(d), z_Float3(d) } };
}

static inline void load_Float3(const iFloat3 d, float *p_out) {
    p_out[0] = x_Float3(d);
    p_out[1] = y_Float3(d);
    p_out[2] = z_Float3(d);
}

static inline iFloat3 yzx_Float3(const iFloat3 d) {
    return iFloatShuffle3(d, 1, 2, 0);
}

static inline iFloat3 zxy_Float3(const iFloat3 d) {
    return iFloatShuffle3(d, 2, 0, 1);
}

static inline iFloat3 xzy_Float3(const iFloat3 d) {
    return iFloatShuffle3(d, 0, 2, 1);
}

static inline void setX_Float3(iFloat3 *d, float x) {
    d->m = _mm_move_ss(d->m, _mm_set_ss(x));
}

static inline void setY_Float3(iFloat3 *d, float y) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(y));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setZ_Float3(iFloat3 *d, float z) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(z));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

typedef iFloat3 iBool3;

static inline iFloat3 add_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_add_ps(a.m, b.m) }; }
static inline iFloat3 sub_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_sub_ps(a.m, b.m) }; }
static inline iFloat3 mul_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_mul_ps(a.m, b.m) }; }
static inline iFloat3 mulf_Float3   (const iFloat3 a, const float b)    { return (iFloat3){ _mm_mul_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat3 fmul_Float3   (const float a, const iFloat3 b)    { return (iFloat3){ _mm_mul_ps(_mm_set1_ps(a), b.m) }; }
static inline iFloat3 div_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_div_ps(a.m, b.m) }; }
static inline iFloat3 divf_Float3   (const iFloat3 a, const float b)    { return (iFloat3){ _mm_div_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat3 fdiv_Float3   (const float a, const iFloat3 b)    { return (iFloat3){ _mm_div_ps(_mm_set1_ps(a), b.m) }; }

static inline iFloat3 addv_Float3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_add_ps(a->m, b.m); return *a; }
static inline iFloat3 subv_Float3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_sub_ps(a->m, b.m); return *a; }
static inline iFloat3 mulv_Float3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_mul_ps(a->m, b.m); return *a; }
static inline iFloat3 mulvf_Float3  (iFloat3 *a, const float b)         { a->m = _mm_mul_ps(a->m, _mm_set1_ps(b)); return *a; }
static inline iFloat3 divv_Float3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_div_ps(a->m, b.m); return *a; }
static inline iFloat3 divvf_Float3  (iFloat3 *a, const float b)         { a->m = _mm_div_ps(a->m, _mm_set1_ps(b)); return *a; }

static inline iBool3 equal_Float3   (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpeq_ps(a.m, b.m) }; }
static inline iBool3 notEqual_Float3(const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpneq_ps(a.m, b.m) }; }
static inline iBool3 less_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmplt_ps(a.m, b.m) }; }
static inline iBool3 greater_Float3 (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpgt_ps(a.m, b.m) }; }
static inline iBool3 lessEqual_Float3   (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmple_ps(a.m, b.m) }; }
static inline iBool3 greaterEqual_Float3(const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpge_ps(a.m, b.m) }; }

static inline iFloat3 min_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_min_ps(a.m, b.m) }; }
static inline iFloat3 max_Float3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_max_ps(a.m, b.m) }; }

static inline iFloat3 neg_Float3    (const iFloat3 a)                   { return (iFloat3){ _mm_sub_ps(_mm_setzero_ps(), a.m) }; }

static inline iFloat3 abs_Float3(const iFloat3 a) {
    return (iFloat3){ _mm_andnot_ps(iFloat4SignBits, a.m) };
}

static inline iFloat3 cross_Float3(const iFloat3 a, const iFloat3 b) {
    return zxy_Float3(sub_Float3(mul_Float3(zxy_Float3(a), b),
                                 mul_Float3(a, zxy_Float3(b))));
}

static inline unsigned mask_Float3  (const iFloat3 a)   { return _mm_movemask_ps(a.m) & 7; }
static inline iBool any_Bool3       (const iBool3 a)    { return mask_Float3(a) != 0; }
static inline iBool all_Bool3       (const iBool3 a)    { return mask_Float3(a) == 7; }

static inline iFloat3 clamp_Float3  (const iFloat3 t, const iFloat3 a, const iFloat3 b) { return min_Float3(max_Float3(t, a), b); }
static inline float sum_Float3      (const iFloat3 a)   { return x_Float3(a) + y_Float3(a) + z_Float3(a); }
static inline float dot_Float3      (const iFloat3 a, const iFloat3 b) { return sum_Float3(mul_Float3(a, b)); }
static inline float lengthSq_Float3 (const iFloat3 a)   { return dot_Float3(a, a); }
static inline float length_Float3   (const iFloat3 a)   { return sqrtf(lengthSq_Float3(a)); }
static inline iFloat3 normalize_Float3(const iFloat3 a) { return mulf_Float3(a, 1.f / length_Float3(a)); }

static inline iFloat3 mix_Float3   (const iFloat3 a, const iFloat3 b, float t) {
    return add_Float3(a, mulf_Float3(sub_Float3(b, a), t));
}
