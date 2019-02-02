#pragma once

/** @file the_Foundation/vec2.h  2D integer vectors.

@authors Copyright (c) 2019 Jaakko Keränen <jaakko.keranen@iki.fi>

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
#include "random.h"
#include <math.h>

iDeclareType(Vec2)

struct Impl_Vec2 {
    int x, y;
};

typedef struct { uint8_t bits; } iBool2;

static inline iVec2 zero_I2(void) {
    return (iVec2){ 0, 0 };
}

static inline iVec2 one_I2(void) {
    return (iVec2){ 1, 1 };
}

static inline iVec2 init1_I2(int x) {
    return (iVec2){ x, x };
}

static inline iVec2 init_I2(int x, int y) {
    return (iVec2){ x, y };
}

static inline iVec2 initu_I2(unsigned x, unsigned y) {
    return (iVec2){ (int) x, (int) y };
}

static inline iVec2 initv_I2(const int *v) {
    return (iVec2){ v[0], v[1] };
}

static inline void store_I2(const iVec2 d, int *p_out) {
    p_out[0] = d.x;
    p_out[1] = d.y;
}

static inline iVec2 yx_I2(const iVec2 d) {
    return (iVec2){ d.y, d.x };
}

static inline iVec2 dx_I2(const iVec2 a, int dx) { return (iVec2){ a.x + dx, a.y }; }
static inline iVec2 dy_I2(const iVec2 a, int dy) { return (iVec2){ a.x, a.y + dy }; }

static inline iVec2 add_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ a.x + b.x, a.y + b.y }; }
static inline iVec2 addi_I2     (const iVec2 a, int b)          { return (iVec2){ a.x + b, a.y + b }; }
static inline iVec2 sub_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ a.x - b.x, a.y - b.y }; }
static inline iVec2 mul_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ a.x * b.x, a.y * b.y }; }
static inline iVec2 muli_I2     (const iVec2 a, int b)          { return (iVec2){ a.x * b, a.y * b }; }
static inline iVec2 mulf_I2     (const iVec2 a, float b)        { return (iVec2){ (int) (a.x * b), (int) (a.y * b) }; }
static inline iVec2 div_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ a.x / b.x, a.y / b.y }; }
static inline iVec2 divi_I2     (const iVec2 a, int b)          { return (iVec2){ a.x / b, a.y / b }; }
static inline iVec2 divf_I2     (const iVec2 a, float b)        { return (iVec2){ (int) (a.x / b), (int) (a.y / b) }; }
static inline iVec2 mod_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ a.x % b.x, a.y % b.y }; }
static inline iVec2 modi_I2     (const iVec2 a, int b)          { return (iVec2){ a.x % b, a.y % b }; }

static inline iVec2 addv_I2     (iVec2 *a, const iVec2 b)       { a->x += b.x; a->y += b.y; return *a; }
static inline iVec2 subv_I2     (iVec2 *a, const iVec2 b)       { a->x -= b.x; a->y -= b.y; return *a; }
static inline iVec2 mulv_I2     (iVec2 *a, const iVec2 b)       { a->x *= b.x; a->y *= b.y; return *a; }
static inline iVec2 muliv_I2    (iVec2 *a, int b)               { a->x *= b; a->y *= b; return *a; }
static inline iVec2 mulfv_I2    (iVec2 *a, float b)             { a->x *= b; a->y *= b; return *a; }
static inline iVec2 divv_I2     (iVec2 *a, const iVec2 b)       { a->x /= b.x; a->y /= b.y; return *a; }
static inline iVec2 diviv_I2    (iVec2 *a, int b)               { a->x /= b; a->y /= b; return *a; }
static inline iVec2 divfv_I2    (iVec2 *a, float b)             { a->x /= b; a->y /= b; return *a; }
static inline iVec2 modv_I2     (iVec2 *a, const iVec2 b)       { a->x %= b.x; a->y %= b.y; return *a; }
static inline iVec2 modiv_I2    (iVec2 *a, int b)               { a->x %= b; a->y %= b; return *a; }

static inline iVec2 min_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ iMin(a.x, b.x), iMin(a.y, b.y) }; }
static inline iVec2 max_I2      (const iVec2 a, const iVec2 b)  { return (iVec2){ iMax(a.x, b.x), iMax(a.y, b.y) }; }
static inline iVec2 neg_I2      (const iVec2 a)                 { return (iVec2){ -a.x, -a.y }; }
static inline iVec2 abs_I2      (const iVec2 a)                 { return (iVec2){ iAbs(a.x), iAbs(a.y) }; }

static inline iBool2 equal_I2   (const iVec2 a, const iVec2 b) {
    return (iBool2){ (a.x == b.x ? 1 : 0) | (a.y == b.y ? 2 : 0) };
}

static inline iBool2 notEqual_I2(const iVec2 a, const iVec2 b) {
    return (iBool2){ (a.x != b.x ? 1 : 0) | (a.y != b.y ? 2 : 0) };
}

static inline iBool2 greater_I2 (const iVec2 a, const iVec2 b) {
    return (iBool2){ (a.x > b.x ? 1 : 0) | (a.y > b.y ? 2 : 0) };
}

static inline iBool2 greaterEqual_I2 (const iVec2 a, const iVec2 b) {
    return (iBool2){ (a.x >= b.x ? 1 : 0) | (a.y >= b.y ? 2 : 0) };
}

static inline iBool2 less_I2 (const iVec2 a, const iVec2 b) {
    return (iBool2){ (a.x < b.x ? 1 : 0) | (a.y < b.y ? 2 : 0) };
}

static inline iBool2 lessEqual_I2 (const iVec2 a, const iVec2 b) {
    return (iBool2){ (a.x <= b.x ? 1 : 0) | (a.y <= b.y ? 2 : 0) };
}

static inline iBool all_Bool2   (const iBool2 a)                { return a.bits == 3; }
static inline iBool any_Bool2   (const iBool2 a)                { return a.bits != 0; }

static inline iBool isEqual_I2  (const iVec2 a, const iVec2 b)  { return a.x == b.x && a.y == b.y; }

static inline iVec2 clamp_I2    (const iVec2 t, const iVec2 a, const iVec2 b) { return min_I2(max_I2(t, a), b); }
static inline int   sum_I2      (const iVec2 a)                 { return a.x + a.y; }
static inline int   prod_I2     (const iVec2 a)                 { return a.x * a.y; }
static inline int   dot_I2      (const iVec2 a, const iVec2 b)  { return sum_I2(mul_I2(a, b)); }
static inline float lengthSq_I2 (const iVec2 a)                 { return dot_I2(a, a); }
static inline float length_I2   (const iVec2 a)                 { return sqrtf(lengthSq_I2(a)); }
static inline float dist_I2     (const iVec2 a, const iVec2 b)  { return length_I2(sub_I2(b, a)); }
static inline int   idist_I2    (const iVec2 a, const iVec2 b)  { return (int) (length_I2(sub_I2(b, a)) + .5f); }

static inline iVec2 mix_I2      (const iVec2 a, const iVec2 b, float t) {
    return add_I2(a, mulf_I2(sub_I2(b, a), t));
}

static inline iVec2 random_I2   (const iVec2 a) { return (iVec2){ iRandom(0, a.x), iRandom(0, a.y) }; }
