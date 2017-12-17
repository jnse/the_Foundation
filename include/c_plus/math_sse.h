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

#include "math.h"

#include <math.h>
#include <smmintrin.h> // SSE 4.1

iDeclareType(Float4)
iDeclareType(Float3)

struct Impl_Float4 {
    __m128 m;
};

static inline iFloat4 zero_F4(void) {
    return (iFloat4){ _mm_setzero_ps() };
}

static inline iFloat4 init1_F4(float x) {
    return (iFloat4){ _mm_set1_ps(x) };
}

static inline iFloat4 init_F4(float x, float y, float z, float w) {
    return (iFloat4){ _mm_set_ps(z, y, x, w) };
}

static inline iFloat4 initi_F4(int x, int y, int z, int w) {
    return init_F4((float) x, (float) y, (float) z, (float) w);
}

static inline iFloat4 initv_F4(const float *v) {
    return (iFloat4){ _mm_loadu_ps(v) };
}

static inline iFloat4 initmm_F4(__m128 m) {
    return (iFloat4){ m };
}

static inline float w_F4(const iFloat4 d) {
    return _mm_cvtss_f32(d.m);
}

static inline float x_F4(const iFloat4 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(1, 1, 1, 1)));
}

static inline float y_F4(const iFloat4 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(2, 2, 2, 2)));
}

static inline float z_F4(const iFloat4 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(3, 3, 3, 3)));
}

static inline void store_F4(const iFloat4 d, float *p_out) {
    _mm_storeu_ps(p_out, _mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(0, 3, 2, 1)));
}

static inline iFloatVec4 values_F4(const iFloat4 d) {
    iFloatVec4 vals;
    store_F4(d, vals.v);
    return vals;
}

#define iFloat4Shuffle3(d, X, Y, Z)     (iFloat4){ _mm_shuffle_ps((d).m, (d).m, _MM_SHUFFLE(Z, Y, X, 0)) }
#define iFloat4Bitmask(x, y, z, w)      (((union { __m128 m; uint32_t m32[4]; }){ .m32 = { z, y, x, w } }).m)
#define iFloat4High92Bits               iFloat4Bitmask(0xffffffff, 0xffffffff, 0xffffffff, 0)

static inline iFloat4 xyz_F4(const iFloat4 d) {
    return (iFloat4){ _mm_and_ps(d.m, iFloat4High92Bits) };
}

static inline iFloat4 yzx_F4(const iFloat4 d) {
    return iFloat4Shuffle3(d, 2, 3, 1);
}

static inline iFloat4 zxy_F4(const iFloat4 d) {
    return iFloat4Shuffle3(d, 3, 1, 2);
}

static inline iFloat4 xzy_F4(const iFloat4 d) {
    return iFloat4Shuffle3(d, 1, 3, 2);
}

static inline void setW_F4(iFloat4 *d, float w) {
    d->m = _mm_move_ss(d->m, _mm_set_ss(w));
}

static inline void setX_F4(iFloat4 *d, float x) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(x));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setY_F4(iFloat4 *d, float y) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(y));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setZ_F4(iFloat4 *d, float z) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(z));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

typedef iFloat4 iBool4;

static inline iFloat4 add_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_add_ps(a.m, b.m) }; }
static inline iFloat4 sub_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_sub_ps(a.m, b.m) }; }
static inline iFloat4 mul_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_mul_ps(a.m, b.m) }; }
static inline iFloat4 mulf_F4   (const iFloat4 a, const float b)    { return (iFloat4){ _mm_mul_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat4 fmul_F4   (const float a, const iFloat4 b)    { return (iFloat4){ _mm_mul_ps(_mm_set1_ps(a), b.m) }; }
static inline iFloat4 div_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_div_ps(a.m, b.m) }; }
static inline iFloat4 divf_F4   (const iFloat4 a, const float b)    { return (iFloat4){ _mm_div_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat4 fdiv_F4   (const float a, const iFloat4 b)    { return (iFloat4){ _mm_div_ps(_mm_set1_ps(a), b.m) }; }

static inline iFloat4 addv_F4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_add_ps(a->m, b.m); return *a; }
static inline iFloat4 subv_F4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_sub_ps(a->m, b.m); return *a; }
static inline iFloat4 mulv_F4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_mul_ps(a->m, b.m); return *a; }
static inline iFloat4 mulvf_F4  (iFloat4 *a, const float b)         { a->m = _mm_mul_ps(a->m, _mm_set1_ps(b)); return *a; }
static inline iFloat4 divv_F4   (iFloat4 *a, const iFloat4 b)       { a->m = _mm_div_ps(a->m, b.m); return *a; }
static inline iFloat4 divvf_F4  (iFloat4 *a, const float b)         { a->m = _mm_div_ps(a->m, _mm_set1_ps(b)); return *a; }

static inline iFloat4 leftv_F4(iFloat4 *a) {
    a->m = _mm_shuffle_ps(a->m, a->m, _MM_SHUFFLE(0, 3, 2, 1));
    return *a;
}

static inline iFloat4 rightv_F4(iFloat4 *a) {
    a->m = _mm_shuffle_ps(a->m, a->m, _MM_SHUFFLE(2, 1, 0, 3));
    return *a;
}

static inline iBool4 equal_F4   (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpeq_ps(a.m, b.m) }; }
static inline iBool4 notEqual_F4(const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpneq_ps(a.m, b.m) }; }
static inline iBool4 less_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmplt_ps(a.m, b.m) }; }
static inline iBool4 greater_F4 (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpgt_ps(a.m, b.m) }; }
static inline iBool4 lessEqual_F4   (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmple_ps(a.m, b.m) }; }
static inline iBool4 greaterEqual_F4(const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_cmpge_ps(a.m, b.m) }; }

static inline iFloat4 min_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_min_ps(a.m, b.m) }; }
static inline iFloat4 max_F4    (const iFloat4 a, const iFloat4 b)  { return (iFloat4){ _mm_max_ps(a.m, b.m) }; }

static inline iFloat4 neg_F4    (const iFloat4 a)                   { return (iFloat4){ _mm_sub_ps(_mm_setzero_ps(), a.m) }; }

#define iFloat4SignBits     iFloat4Bitmask(0x80000000, 0x80000000, 0x80000000, 0x80000000)

static inline iFloat4 abs_F4(const iFloat4 a) {
    return (iFloat4){ _mm_andnot_ps(iFloat4SignBits, a.m) };
}

static inline unsigned mask_F4  (const iFloat4 a)   { return _mm_movemask_ps(a.m) & 15; }
static inline iBool any_Bool4   (const iBool4 a)    { return mask_F4(a) != 0; }
static inline iBool all_Bool4   (const iBool4 a)    { return mask_F4(a) == 15; }

static inline iFloat4 clamp_F4  (const iFloat4 t, const iFloat4 a, const iFloat4 b) { return min_F4(max_F4(t, a), b); }
static inline float sum_F4      (const iFloat4 a)   { return x_F4(a) + y_F4(a) + z_F4(a) + w_F4(a); }
static inline float dot_F4      (const iFloat4 a, const iFloat4 b) { return _mm_cvtss_f32(_mm_dp_ps(a.m, b.m, 0xf1)); }
static inline float lengthSq_F4 (const iFloat4 a)   { return dot_F4(a, a); }
static inline float length_F4   (const iFloat4 a)   { return sqrtf(lengthSq_F4(a)); }
static inline iFloat4 normalize_F4(const iFloat4 a) { return mulf_F4(a, 1.f / length_F4(a)); }
static inline iFloat4 sqrt_F4   (const iFloat4 a)   { return (iFloat4){ _mm_sqrt_ps(a.m) }; }

static inline iFloat4 mix_F4   (const iFloat4 a, const iFloat4 b, float t) {
    return add_F4(a, mulf_F4(sub_F4(b, a), t));
}

//---------------------------------------------------------------------------------------

struct Impl_Float3 {
    __m128 m;
};

static inline iFloat3 zero_F3(void) {
    return (iFloat3){ _mm_setzero_ps() };
}

static inline iFloat3 init1_F3(float x) {
    return (iFloat3){ _mm_set1_ps(x) };
}

static inline iFloat3 init_F3(float x, float y, float z) {
    return (iFloat3){ _mm_set_ps(z, y, x, 0.f) };
}

static inline iFloat3 initi_F3(int x, int y, int z) {
    return init_F3((float) x, (float) y, (float) z);
}

static inline iFloat3 initv_F3(const float *v) {
    return (iFloat3){ _mm_set_ps(v[2], v[1], v[0], 0.f) };
}

static inline float x_F3(const iFloat3 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(1, 1, 1, 1)));
}

static inline float y_F3(const iFloat3 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(2, 2, 2, 2)));
}

static inline float z_F3(const iFloat3 d) {
    return _mm_cvtss_f32(_mm_shuffle_ps(d.m, d.m, _MM_SHUFFLE(3, 3, 3, 3)));
}

static inline void store_F3(const iFloat3 d, float *p_out) {
    _Alignas(16) float t[4];
    _mm_store_ps(t, d.m);
    p_out[0] = t[1];
    p_out[1] = t[2];
    p_out[2] = t[3];
}

static inline iFloatVec3 values_F3(const iFloat3 d) {
    iFloatVec3 vec3;
    store_F3(d, vec3.v);
    return vec3;
}

#define iFloat3Shuffle3(d, X, Y, Z)  (iFloat3){ _mm_shuffle_ps((d).m, (d).m, _MM_SHUFFLE(Z, Y, X, 0)) }

static inline iFloat3 yzx_F3(const iFloat3 d) {
    return iFloat3Shuffle3(d, 2, 3, 1);
}

static inline iFloat3 zxy_F3(const iFloat3 d) {
    return iFloat3Shuffle3(d, 3, 1, 2);
}

static inline iFloat3 xzy_F3(const iFloat3 d) {
    return iFloat3Shuffle3(d, 1, 3, 2);
}

static inline void setX_F3(iFloat3 *d, float x) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(x));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 2, 0, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setY_F3(iFloat3 *d, float y) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(y));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(3, 0, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

static inline void setZ_F3(iFloat3 *d, float z) {
    __m128 t = _mm_move_ss(d->m, _mm_set_ss(z));
    t = _mm_shuffle_ps(t, t, _MM_SHUFFLE(0, 2, 1, 0));
    d->m = _mm_move_ss(t, d->m);
}

typedef iFloat3 iBool3;

static inline iFloat3 add_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_add_ps(a.m, b.m) }; }
static inline iFloat3 sub_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_sub_ps(a.m, b.m) }; }
static inline iFloat3 mul_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_mul_ps(a.m, b.m) }; }
static inline iFloat3 mulf_F3   (const iFloat3 a, const float b)    { return (iFloat3){ _mm_mul_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat3 fmul_F3   (const float a, const iFloat3 b)    { return (iFloat3){ _mm_mul_ps(_mm_set1_ps(a), b.m) }; }
static inline iFloat3 div_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_div_ps(a.m, b.m) }; }
static inline iFloat3 divf_F3   (const iFloat3 a, const float b)    { return (iFloat3){ _mm_div_ps(a.m, _mm_set1_ps(b)) }; }
static inline iFloat3 fdiv_F3   (const float a, const iFloat3 b)    { return (iFloat3){ _mm_div_ps(_mm_set1_ps(a), b.m) }; }

static inline iFloat3 addv_F3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_add_ps(a->m, b.m); return *a; }
static inline iFloat3 subv_F3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_sub_ps(a->m, b.m); return *a; }
static inline iFloat3 mulv_F3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_mul_ps(a->m, b.m); return *a; }
static inline iFloat3 mulvf_F3  (iFloat3 *a, const float b)         { a->m = _mm_mul_ps(a->m, _mm_set1_ps(b)); return *a; }
static inline iFloat3 divv_F3   (iFloat3 *a, const iFloat3 b)       { a->m = _mm_div_ps(a->m, b.m); return *a; }
static inline iFloat3 divvf_F3  (iFloat3 *a, const float b)         { a->m = _mm_div_ps(a->m, _mm_set1_ps(b)); return *a; }

static inline iBool3 equal_F3   (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpeq_ps(a.m, b.m) }; }
static inline iBool3 notEqual_F3(const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpneq_ps(a.m, b.m) }; }
static inline iBool3 less_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmplt_ps(a.m, b.m) }; }
static inline iBool3 greater_F3 (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpgt_ps(a.m, b.m) }; }
static inline iBool3 lessEqual_F3   (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmple_ps(a.m, b.m) }; }
static inline iBool3 greaterEqual_F3(const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_cmpge_ps(a.m, b.m) }; }

static inline iFloat3 min_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_min_ps(a.m, b.m) }; }
static inline iFloat3 max_F3    (const iFloat3 a, const iFloat3 b)  { return (iFloat3){ _mm_max_ps(a.m, b.m) }; }

static inline iFloat3 neg_F3    (const iFloat3 a)                   { return (iFloat3){ _mm_sub_ps(_mm_setzero_ps(), a.m) }; }

static inline iFloat3 abs_F3(const iFloat3 a) {
    return (iFloat3){ _mm_andnot_ps(iFloat4SignBits, a.m) };
}

static inline iFloat3 cross_F3(const iFloat3 a, const iFloat3 b) {
    return zxy_F3(sub_F3(mul_F3(zxy_F3(a), b),
                         mul_F3(a, zxy_F3(b))));
}

static inline unsigned mask_F3  (const iFloat3 a)   { return _mm_movemask_ps(a.m) & (8|4|2); }
static inline iBool any_Bool3   (const iBool3 a)    { return mask_F3(a) != 0; }
static inline iBool all_Bool3   (const iBool3 a)    { return mask_F3(a) == (8|4|2); }

static inline iFloat3 clamp_F3  (const iFloat3 t, const iFloat3 a, const iFloat3 b) { return min_F3(max_F3(t, a), b); }
static inline float sum_F3      (const iFloat3 a)       { return x_F3(a) + y_F3(a) + z_F3(a); }
static inline float dot_F3      (const iFloat3 a, const iFloat3 b) { return _mm_cvtss_f32(_mm_dp_ps(a.m, b.m, 0xe1)); }
static inline float lengthSq_F3 (const iFloat3 a)       { return dot_F3(a, a); }
static inline float length_F3   (const iFloat3 a)       { return sqrtf(lengthSq_F3(a)); }
static inline iFloat3 normalize_F3(const iFloat3 a)     { return mulf_F3(a, 1.f / length_F3(a)); }
static inline iFloat3 sqrt_F3   (const iFloat3 a)       { return (iFloat3){ _mm_sqrt_ps(a.m) }; }

static inline iFloat3 mix_F3   (const iFloat3 a, const iFloat3 b, float t) {
    return add_F3(a, mulf_F3(sub_F3(b, a), t));
}

//---------------------------------------------------------------------------------------

iDeclareType(Mat4)

struct Impl_Mat4 {
    __m128 col[4];
};

void init_Mat4  (iMat4 *);

void store_Mat4 (const iMat4 *, float *v);

static inline void copy_Mat4(iMat4 *d, const iMat4 *other) {
    d->col[0] = other->col[0];
    d->col[1] = other->col[1];
    d->col[2] = other->col[2];
    d->col[3] = other->col[3];
}

void mul_Mat4(iMat4 *, const iMat4 *b);

static inline void translate_Mat4(iMat4 *d, iFloat3 v) {
    d->col[0] = _mm_add_ss(d->col[0], _mm_shuffle_ps(v.m, v.m, _MM_SHUFFLE(3, 2, 1, 1)));
    d->col[1] = _mm_add_ss(d->col[1], _mm_shuffle_ps(v.m, v.m, _MM_SHUFFLE(3, 2, 1, 2)));
    d->col[2] = _mm_add_ss(d->col[2], _mm_shuffle_ps(v.m, v.m, _MM_SHUFFLE(3, 2, 1, 3)));
}

static inline void initTranslate_Mat4(iMat4 *d, iFloat3 v) {
    init_Mat4(d);
    translate_Mat4(d, v);
}

static inline void initScale_Mat4(iMat4 *d, iFloat3 v) {
    d->col[0] = _mm_set_ps(0, 0, x_F3(v), 0);
    d->col[1] = _mm_set_ps(0, y_F3(v), 0, 0);
    d->col[2] = _mm_set_ps(z_F3(v), 0, 0, 0);
    d->col[3] = _mm_set_ps(0, 0, 0, 1);
}

static inline void scale_Mat4(iMat4 *d, iFloat3 v) {
    d->col[0] = _mm_mul_ps(d->col[0], _mm_set_ps(1, 1, x_F3(v), 1));
    d->col[1] = _mm_mul_ps(d->col[1], _mm_set_ps(1, y_F3(v), 1, 1));
    d->col[2] = _mm_mul_ps(d->col[2], _mm_set_ps(z_F3(v), 1, 1, 1));
}

static inline void scalef_Mat4(iMat4 *d, float v) {
    d->col[0] = _mm_mul_ps(d->col[0], _mm_set_ps(1, 1, v, 1));
    d->col[1] = _mm_mul_ps(d->col[1], _mm_set_ps(1, v, 1, 1));
    d->col[2] = _mm_mul_ps(d->col[2], _mm_set_ps(v, 1, 1, 1));
}

void initRotate_Mat4(iMat4 *d, iFloat3 axis, float degrees);

static inline void rotate_Mat4(iMat4 *d, iFloat3 axis, float degrees) {
    iMat4 rot; initRotate_Mat4(&rot, axis, degrees);
    mul_Mat4(d, &rot);
}

static inline iFloat4 mulF4_Mat4(const iMat4 *d, iFloat4 v) {
    return init_F4(dot_F4(initmm_F4(d->col[0]), v),
                   dot_F4(initmm_F4(d->col[1]), v),
                   dot_F4(initmm_F4(d->col[2]), v),
                   dot_F4(initmm_F4(d->col[3]), v));
}

static inline iFloat3 mulF3_Mat4(const iMat4 *d, const iFloat3 v) {
    iFloat4 i = mulF4_Mat4(d, initmm_F4(_mm_move_ss(v.m, _mm_set1_ps(1.f))));
    return (iFloat3){ _mm_div_ps(i.m, _mm_set1_ps(_mm_cvtss_f32(i.m))) };
}
