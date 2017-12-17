#pragma once

/** @file math_generic.h  Vector math, generic implementation.
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

typedef iFloatVec3 iFloat3;
typedef iFloatVec4 iFloat4;

static inline iFloat4 zero_F4(void) {
    return (iFloat4){ .v = { 0, 0, 0, 0 } };
}

static inline iFloat4 init1_F4(float x) {
    return (iFloat4){ .v = { x, x, x, x } };
}

static inline iFloat4 init_F4(float x, float y, float z, float w) {
    return (iFloat4){ .v = { x, y, z, w } };
}

static inline iFloat4 initi_F4(int x, int y, int z, int w) {
    return (iFloat4){ .v = { (float) x, (float) y, (float) z, (float) w } };
}

static inline iFloat4 initv_F4(const float *v) {
    return (iFloat4){ .v = { v[0], v[1], v[2], v[3] } };
}

static inline float x_F4(const iFloat4 d) {
    return d.value.x;
}

static inline float y_F4(const iFloat4 d) {
    return d.value.y;
}

static inline float z_F4(const iFloat4 d) {
    return d.value.z;
}

static inline float w_F4(const iFloat4 d) {
    return d.value.w;
}

static inline void store_F4(const iFloat4 d, float *p_out) {
    p_out[0] = d.v[0];
    p_out[1] = d.v[1];
    p_out[2] = d.v[2];
    p_out[3] = d.v[3];
}

static inline iFloatVec4 values_F4(const iFloat4 d) {
    return d;
}

static inline iFloat4 shuffle_F4(const iFloat4 d, int x, int y, int z, int w) {
    return init_F4(d.v[x], d.v[y], d.v[z], d.v[w]);
}

static inline iFloat4 xyz_F4(const iFloat4 d) {
    return init_F4(d.value.x, d.value.y, d.value.z, 0);
}

static inline iFloat4 yzx_F4(const iFloat4 d) {
    return shuffle_F4(d, 1, 2, 0, 3);
}

static inline iFloat4 zxy_F4(const iFloat4 d) {
    return shuffle_F4(d, 2, 0, 1, 3);
}

static inline iFloat4 xzy_F4(const iFloat4 d) {
    return shuffle_F4(d, 0, 2, 1, 3);
}

static inline void setX_F4(iFloat4 *d, float x) {
    d->value.x = x;
}

static inline void setY_F4(iFloat4 *d, float y) {
    d->value.y = y;
}

static inline void setZ_F4(iFloat4 *d, float z) {
    d->value.z = z;
}

static inline void setW_F4(iFloat4 *d, float w) {
    d->value.w = w;
}

typedef iFloat4 iBool4;

static inline iFloat4 add_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { a.value.x + b.value.x, a.value.y + b.value.y, a.value.z + b.value.z, a.value.w + b.value.w } };
}

static inline iFloat4 sub_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { a.value.x - b.value.x, a.value.y - b.value.y, a.value.z - b.value.z, a.value.w - b.value.w } };
}

static inline iFloat4 mul_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { a.value.x * b.value.x, a.value.y * b.value.y, a.value.z * b.value.z, a.value.w * b.value.w } };
}

static inline iFloat4 mulf_F4   (const iFloat4 a, const float b)    {
    return (iFloat4){ .v = { a.value.x * b, a.value.y * b, a.value.z * b, a.value.w * b } };
}

static inline iFloat4 fmul_F4   (const float a, const iFloat4 b)    {
    return (iFloat4){ .v = { b.value.x * a, b.value.y * a, b.value.z * a, b.value.w * a } };
}

static inline iFloat4 div_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { a.value.x / b.value.x, a.value.y / b.value.y, a.value.z / b.value.z, a.value.w / b.value.w } };
}

static inline iFloat4 divf_F4   (const iFloat4 a, const float b)    {
    return (iFloat4){ .v = { a.value.x / b, a.value.y / b, a.value.z / b, a.value.w / b } };
}

static inline iFloat4 fdiv_F4   (const float a, const iFloat4 b)    {
    return (iFloat4){ .v = { b.value.x / a, b.value.y / a, b.value.z / a, b.value.w / a } };
}

static inline iFloat4 addv_F4   (iFloat4 *a, const iFloat4 b)       {
    a->v[0] += b.v[0];
    a->v[1] += b.v[1];
    a->v[2] += b.v[2];
    a->v[3] += b.v[3];
    return *a;
}

static inline iFloat4 subv_F4   (iFloat4 *a, const iFloat4 b)       {
    a->v[0] -= b.v[0];
    a->v[1] -= b.v[1];
    a->v[2] -= b.v[2];
    a->v[3] -= b.v[3];
    return *a;
}

static inline iFloat4 mulv_F4   (iFloat4 *a, const iFloat4 b)       {
    a->v[0] *= b.v[0];
    a->v[1] *= b.v[1];
    a->v[2] *= b.v[2];
    a->v[3] *= b.v[3];
    return *a;
}

static inline iFloat4 mulvf_F4  (iFloat4 *a, const float b)         {
    a->v[0] *= b;
    a->v[1] *= b;
    a->v[2] *= b;
    a->v[3] *= b;
    return *a;
}

static inline iFloat4 divv_F4   (iFloat4 *a, const iFloat4 b)       {
    a->v[0] /= b.v[0];
    a->v[1] /= b.v[1];
    a->v[2] /= b.v[2];
    a->v[3] /= b.v[3];
    return *a;
}

static inline iFloat4 divvf_F4  (iFloat4 *a, const float b)         {
    a->v[0] /= b;
    a->v[1] /= b;
    a->v[2] /= b;
    a->v[3] /= b;
    return *a;
}

static inline iFloat4 leftv_F4(iFloat4 *a) {
    const float t = a->v[3];
    a->v[3] = a->v[0];
    a->v[0] = a->v[1];
    a->v[1] = a->v[2];
    a->v[2] = t;
    return *a;
}

static inline iFloat4 rightv_F4(iFloat4 *a) {
    const float t = a->v[3];
    a->v[3] = a->v[2];
    a->v[2] = a->v[1];
    a->v[1] = a->v[0];
    a->v[0] = t;
    return *a;
}

#define iFloatVecBool(b)   ((b)? 1.0f : 0.0f)

static inline iBool4 equal_F4   (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iFloatVecBool(a.v[0] == b.v[0]),
                             iFloatVecBool(a.v[1] == b.v[1]),
                             iFloatVecBool(a.v[2] == b.v[2]),
                             iFloatVecBool(a.v[3] == b.v[3]) } };
}

static inline iBool4 notEqual_F4(const iFloat4 a, const iFloat4 b)  {
    return sub_F4(init1_F4(1.f), equal_F4(a, b));
}

static inline iBool4 less_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iFloatVecBool(a.v[0] < b.v[0]),
                             iFloatVecBool(a.v[1] < b.v[1]),
                             iFloatVecBool(a.v[2] < b.v[2]),
                             iFloatVecBool(a.v[3] < b.v[3]) } };
}

static inline iBool4 greater_F4 (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iFloatVecBool(a.v[0] > b.v[0]),
                             iFloatVecBool(a.v[1] > b.v[1]),
                             iFloatVecBool(a.v[2] > b.v[2]),
                             iFloatVecBool(a.v[3] > b.v[3]) } };
}

static inline iBool4 lessEqual_F4   (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iFloatVecBool(a.v[0] <= b.v[0]),
                             iFloatVecBool(a.v[1] <= b.v[1]),
                             iFloatVecBool(a.v[2] <= b.v[2]),
                             iFloatVecBool(a.v[3] <= b.v[3]) } };
}

static inline iBool4 greaterEqual_F4(const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iFloatVecBool(a.v[0] >= b.v[0]),
                             iFloatVecBool(a.v[1] >= b.v[1]),
                             iFloatVecBool(a.v[2] >= b.v[2]),
                             iFloatVecBool(a.v[3] >= b.v[3]) } };
}

static inline iFloat4 min_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iMin(a.v[0], b.v[0]),
                             iMin(a.v[1], b.v[1]),
                             iMin(a.v[2], b.v[2]),
                             iMin(a.v[3], b.v[3]) } };
}

static inline iFloat4 max_F4    (const iFloat4 a, const iFloat4 b)  {
    return (iFloat4){ .v = { iMax(a.v[0], b.v[0]),
                             iMax(a.v[1], b.v[1]),
                             iMax(a.v[2], b.v[2]),
                             iMax(a.v[3], b.v[3]) } };
}

static inline iFloat4 neg_F4    (const iFloat4 a)                   {
    return sub_F4(zero_F4(), a);
}

static inline iFloat4 abs_F4(const iFloat4 a) {
    return (iFloat4){ .v = { fabsf(a.v[0]),
                             fabsf(a.v[1]),
                             fabsf(a.v[2]),
                             fabsf(a.v[3]) } };
}

static inline unsigned mask_F4  (const iFloat4 a)   {
    return (a.v[0] > 0? 0x1 : 0) |
           (a.v[1] > 0? 0x2 : 0) |
           (a.v[2] > 0? 0x4 : 0) |
           (a.v[3] > 0? 0x8 : 0);
}

static inline iBool any_Bool4       (const iBool4 a)    { return mask_F4(a) != 0; }
static inline iBool all_Bool4       (const iBool4 a)    { return mask_F4(a) == 15; }

static inline iFloat4 clamp_F4  (const iFloat4 t, const iFloat4 a, const iFloat4 b) { return min_F4(max_F4(t, a), b); }
static inline float sum_F4      (const iFloat4 a)   { return x_F4(a) + y_F4(a) + z_F4(a) + w_F4(a); }
static inline float dot_F4      (const iFloat4 a, const iFloat4 b) { return sum_F4(mul_F4(a, b)); }
static inline float lengthSq_F4 (const iFloat4 a)   { return dot_F4(a, a); }
static inline float length_F4   (const iFloat4 a)   { return sqrtf(lengthSq_F4(a)); }
static inline iFloat4 normalize_F4(const iFloat4 a) { return mulf_F4(a, 1.f / length_F4(a)); }
static inline iFloat4 sqrt_F4   (const iFloat4 a)   {
    return (iFloat4){ .v = { sqrtf(a.v[0]), sqrtf(a.v[1]), sqrtf(a.v[2]), sqrtf(a.v[3]) } };
}

static inline iFloat4 mix_F4   (const iFloat4 a, const iFloat4 b, float t) {
    return add_F4(a, mulf_F4(sub_F4(b, a), t));
}

//---------------------------------------------------------------------------------------

static inline iFloat3 zero_F3(void) {
    return (iFloat3){ .v = { 0, 0, 0 } };
}

static inline iFloat3 init1_F3(float x) {
    return (iFloat3){ .v = { x, x, x } };
}

static inline iFloat3 init_F3(float x, float y, float z) {
    return (iFloat3){ .v = { x, y, z } };
}

static inline iFloat3 initi_F3(int x, int y, int z) {
    return (iFloat3){ .v = { (float) x, (float) y, (float) z } };
}

static inline iFloat3 initv_F3(const float *v) {
    return (iFloat3){ .v = { v[0], v[1], v[2] } };
}

static inline float x_F3(const iFloat3 d) {
    return d.value.x;
}

static inline float y_F3(const iFloat3 d) {
    return d.value.y;
}

static inline float z_F3(const iFloat3 d) {
    return d.value.z;
}

static inline void store_F3(const iFloat3 d, float *p_out) {
    p_out[0] = d.v[0];
    p_out[1] = d.v[1];
    p_out[2] = d.v[2];
}

static inline iFloatVec3 values_F3(const iFloat3 d) {
    return d;
}

static inline iFloat3 shuffle_F3(const iFloat3 d, int x, int y, int z) {
    return init_F3(d.v[x], d.v[y], d.v[z]);
}

static inline iFloat3 xyz_F3(const iFloat3 d) {
    return init_F3(d.value.x, d.value.y, d.value.z);
}

static inline iFloat3 yzx_F3(const iFloat3 d) {
    return shuffle_F3(d, 1, 2, 0);
}

static inline iFloat3 zxy_F3(const iFloat3 d) {
    return shuffle_F3(d, 2, 0, 1);
}

static inline iFloat3 xzy_F3(const iFloat3 d) {
    return shuffle_F3(d, 0, 2, 1);
}

static inline void setX_F3(iFloat3 *d, float x) {
    d->value.x = x;
}

static inline void setY_F3(iFloat3 *d, float y) {
    d->value.y = y;
}

static inline void setZ_F3(iFloat3 *d, float z) {
    d->value.z = z;
}

typedef iFloat3 iBool3;

static inline iFloat3 add_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iFloat3){ .v = { a.value.x + b.value.x, a.value.y + b.value.y, a.value.z + b.value.z } };
}

static inline iFloat3 sub_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iFloat3){ .v = { a.value.x - b.value.x, a.value.y - b.value.y, a.value.z - b.value.z } };
}

static inline iFloat3 mul_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iFloat3){ .v = { a.value.x * b.value.x, a.value.y * b.value.y, a.value.z * b.value.z } };
}

static inline iFloat3 mulf_F3   (const iFloat3 a, const float b)    {
    return (iFloat3){ .v = { a.value.x * b, a.value.y * b, a.value.z * b } };
}

static inline iFloat3 fmul_F3   (const float a, const iFloat3 b)    {
    return (iFloat3){ .v = { b.value.x * a, b.value.y * a, b.value.z * a } };
}

static inline iFloat3 div_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iFloat3){ .v = { a.value.x / b.value.x, a.value.y / b.value.y, a.value.z / b.value.z } };
}

static inline iFloat3 divf_F3   (const iFloat3 a, const float b)    {
    return (iFloat3){ .v = { a.value.x / b, a.value.y / b, a.value.z / b } };
}

static inline iFloat3 fdiv_F3   (const float a, const iFloat3 b)    {
    return (iFloat3){ .v = { b.value.x / a, b.value.y / a, b.value.z / a } };
}

static inline iFloat3 addv_F3   (iFloat3 *a, const iFloat3 b)       {
    a->v[0] += b.v[0];
    a->v[1] += b.v[1];
    a->v[2] += b.v[2];
    return *a;
}

static inline iFloat3 subv_F3   (iFloat3 *a, const iFloat3 b)       {
    a->v[0] -= b.v[0];
    a->v[1] -= b.v[1];
    a->v[2] -= b.v[2];
    return *a;
}

static inline iFloat3 mulv_F3   (iFloat3 *a, const iFloat3 b)       {
    a->v[0] *= b.v[0];
    a->v[1] *= b.v[1];
    a->v[2] *= b.v[2];
    return *a;
}

static inline iFloat3 mulvf_F3  (iFloat3 *a, const float b)         {
    a->v[0] *= b;
    a->v[1] *= b;
    a->v[2] *= b;
    return *a;
}

static inline iFloat3 divv_F3   (iFloat3 *a, const iFloat3 b)       {
    a->v[0] /= b.v[0];
    a->v[1] /= b.v[1];
    a->v[2] /= b.v[2];
    return *a;
}

static inline iFloat3 divvf_F3  (iFloat3 *a, const float b)         {
    a->v[0] /= b;
    a->v[1] /= b;
    a->v[2] /= b;
    return *a;
}

static inline iFloat3 leftv_F3(iFloat3 *a) {
    const float t = a->v[2];
    a->v[2] = a->v[0];
    a->v[0] = a->v[1];
    a->v[1] = t;
    return *a;
}

static inline iFloat3 rightv_F3(iFloat3 *a) {
    const float t = a->v[2];
    a->v[2] = a->v[1];
    a->v[1] = a->v[0];
    a->v[0] = t;
    return *a;
}

static inline iBool3 equal_F3   (const iFloat3 a, const iFloat3 b)  {
    return (iBool3){ .v = { iFloatVecBool(a.v[0] == b.v[0]),
                             iFloatVecBool(a.v[1] == b.v[1]),
                             iFloatVecBool(a.v[2] == b.v[2]) } };
}

static inline iBool3 notEqual_F3(const iFloat3 a, const iFloat3 b)  {
    return sub_F3(init1_F3(1.f), equal_F3(a, b));
}

static inline iBool3 less_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iBool3){ .v = { iFloatVecBool(a.v[0] < b.v[0]),
                            iFloatVecBool(a.v[1] < b.v[1]),
                            iFloatVecBool(a.v[2] < b.v[2]) } };
}

static inline iBool3 greater_F3 (const iFloat3 a, const iFloat3 b)  {
    return (iBool3){ .v = { iFloatVecBool(a.v[0] > b.v[0]),
                            iFloatVecBool(a.v[1] > b.v[1]),
                            iFloatVecBool(a.v[2] > b.v[2]) } };
}

static inline iBool3 lessEqual_F3   (const iFloat3 a, const iFloat3 b)  {
    return (iBool3){ .v = { iFloatVecBool(a.v[0] <= b.v[0]),
                            iFloatVecBool(a.v[1] <= b.v[1]),
                            iFloatVecBool(a.v[2] <= b.v[2]) } };
}

static inline iBool3 greaterEqual_F3(const iFloat3 a, const iFloat3 b)  {
    return (iBool3){ .v = { iFloatVecBool(a.v[0] >= b.v[0]),
                            iFloatVecBool(a.v[1] >= b.v[1]),
                            iFloatVecBool(a.v[2] >= b.v[2]) } };
}

static inline iFloat3 min_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iFloat3){ .v = { iMin(a.v[0], b.v[0]),
                             iMin(a.v[1], b.v[1]),
                             iMin(a.v[2], b.v[2]) } };
}

static inline iFloat3 max_F3    (const iFloat3 a, const iFloat3 b)  {
    return (iFloat3){ .v = { iMax(a.v[0], b.v[0]),
                             iMax(a.v[1], b.v[1]),
                             iMax(a.v[2], b.v[2]) } };
}

static inline iFloat3 neg_F3    (const iFloat3 a)                   {
    return sub_F3(zero_F3(), a);
}

static inline iFloat3 abs_F3(const iFloat3 a) {
    return (iFloat3){ .v = { fabsf(a.v[0]),
                             fabsf(a.v[1]),
                             fabsf(a.v[2]) } };
}

static inline unsigned mask_F3  (const iFloat3 a)   {
    return (a.v[0] > 0? 0x1 : 0) |
           (a.v[1] > 0? 0x2 : 0) |
           (a.v[2] > 0? 0x4 : 0);
}

static inline iBool any_Bool3       (const iBool3 a)    { return mask_F3(a) != 0; }
static inline iBool all_Bool3       (const iBool3 a)    { return mask_F3(a) == 7; }

static inline iFloat3 clamp_F3  (const iFloat3 t, const iFloat3 a, const iFloat3 b) { return min_F3(max_F3(t, a), b); }
static inline float sum_F3      (const iFloat3 a)   { return x_F3(a) + y_F3(a) + z_F3(a); }
static inline float dot_F3      (const iFloat3 a, const iFloat3 b) { return sum_F3(mul_F3(a, b)); }
static inline float lengthSq_F3 (const iFloat3 a)   { return dot_F3(a, a); }
static inline float length_F3   (const iFloat3 a)   { return sqrtf(lengthSq_F3(a)); }
static inline iFloat3 normalize_F3(const iFloat3 a) { return mulf_F3(a, 1.f / length_F3(a)); }
static inline iFloat3 sqrt_F3   (const iFloat3 a)   {
    return (iFloat3){ .v = { sqrtf(a.v[0]), sqrtf(a.v[1]), sqrtf(a.v[2]) } };
}

static inline iFloat3 mix_F3   (const iFloat3 a, const iFloat3 b, float t) {
    return add_F3(a, mulf_F3(sub_F3(b, a), t));
}

static inline iFloat3 cross_F3(const iFloat3 a, const iFloat3 b) {
    return zxy_F3(sub_F3(mul_F3(zxy_F3(a), b),
                         mul_F3(a, zxy_F3(b))));
}

//---------------------------------------------------------------------------------------

iDeclareType(Mat4)

struct Impl_Mat4 {
    iFloatVec4 col[4];
};

void init_Mat4  (iMat4 *);

void store_Mat4 (const iMat4 *, float *v);

static inline void copy_Mat4(iMat4 *d, const iMat4 *other) {
    d->col[0] = other->col[0];
    d->col[1] = other->col[1];
    d->col[2] = other->col[2];
    d->col[3] = other->col[3];
}

void mul_Mat4(iMat4 *, const iMat4 *b); //, iMat4 *m_out);

static inline void translate_Mat4(iMat4 *d, iFloat3 v) {
    d->col[0].value.w += v.v[0];
    d->col[1].value.w += v.v[1];
    d->col[2].value.w += v.v[2];
}

static inline void initTranslate_Mat4(iMat4 *d, iFloat3 v) {
    init_Mat4(d);
    translate_Mat4(d, v);
}

static inline void initScale_Mat4(iMat4 *d, iFloat3 v) {
    d->col[0] = init_F4(x_F3(v), 0, 0, 0);
    d->col[1] = init_F4(0, y_F3(v), 0, 0);
    d->col[2] = init_F4(0, 0, z_F3(v), 0);
    d->col[3] = init_F4(0, 0, 0, 1);
}

static inline void scale_Mat4(iMat4 *d, iFloat3 v) {
    d->col[0].value.x *= v.value.x;
    d->col[1].value.y *= v.value.y;
    d->col[2].value.z *= v.value.z;
}

static inline void initRotate_Mat4(iMat4 *d, iFloat3 axis, float degrees) {

}

static inline void scalef_Mat4(iMat4 *d, float v) {
    d->col[0].value.x *= v;
    d->col[1].value.y *= v;
    d->col[2].value.z *= v;
}

static inline iFloat4 mulF4_Mat4(const iMat4 *d, const iFloat4 v) {
    return init_F4(dot_F4(d->col[0], v),
                   dot_F4(d->col[1], v),
                   dot_F4(d->col[2], v),
                   dot_F4(d->col[3], v));
}

static inline iFloat3 mulF3_Mat4(const iMat4 *d, const iFloat3 v) {
    iFloat4 v4 = mulF4_Mat4(d, init_F4(v.v[0], v.v[1], v.v[2], 1.0f));
    return divf_F3(initv_F3(v4.v), v4.value.w);
}

