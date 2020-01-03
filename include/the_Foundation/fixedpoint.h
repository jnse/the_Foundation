#pragma once

/** @file fixedpoint.h  Fixed-point 48.16 math routines.

@authors Copyright (c) 2020 Jaakko Keränen <jaakko.keranen@iki.fi>

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

iDeclareType(Fixed)

typedef int64_t     iFixed64;
typedef __int128_t  iFixedLong;

struct Impl_Fixed {
    union {
        iFixed64 v;
        struct {
            uint64_t frac : 16;
            uint64_t wnum : 47;
            uint64_t sign : 1;
        } comp;
    };
};

#define iFixedFracBits  16
#define iFixedUnit      (1 << iFixedFracBits)
#define iFixedMaxWNum   ((1UL << 47) - 1)

iLocalDef iFixed zero_Fixed(void) {
    return (iFixed){ .v = 0 };
}

iLocalDef iFixed one_Fixed(void) {
    return (iFixed){ .v = iFixedUnit };
}

iLocalDef iFixed half_Fixed(void) {
    return (iFixed){ .v = iFixedUnit >> 1 };
}

iLocalDef iFixed init_Fixed(iFixed64 fp) {
    return (iFixed){ .v = fp };
}

iLocalDef iFixed initi_Fixed(int32_t i) {
    return (iFixed){ .v = i << iFixedFracBits };
}

iLocalDef iFixed initf_Fixed(float f) {
    return (iFixed){ .v = (iFixed64) (f * iFixedUnit) };
}

iLocalDef iFixed initd_Fixed(double d) {
    return (iFixed){ .v = (iFixed64) (d * iFixedUnit) };
}

iLocalDef iFixed add_Fixed(const iFixed a, const iFixed b) {
    return (iFixed){ .v = a.v + b.v };
}

iLocalDef void addv_Fixed(iFixed *a, const iFixed b) {
    a->v += b.v;
}

iLocalDef iFixed sub_Fixed(const iFixed a, const iFixed b) {
    return (iFixed){ .v = a.v - b.v };
}

iLocalDef void subv_Fixed(iFixed *a, const iFixed b) {
    a->v -= b.v;
}

iLocalDef iFixed mul_Fixed(const iFixed a, const iFixed b) {
    return init_Fixed((((iFixedLong) a.v * (iFixedLong) b.v) >> iFixedFracBits));
}

iLocalDef void mulv_Fixed(iFixed *a, const iFixed b) {
    *a = mul_Fixed(*a, b);
}

iLocalDef iFixed muli_Fixed(const iFixed a, int i) {
    return init_Fixed(a.v * i);
}

iLocalDef iFixed mulf_Fixed(const iFixed a, float f) {
    return init_Fixed((iFixed64) (a.v * f));
}

iLocalDef iFixed div_Fixed(const iFixed a, const iFixed b) {
    return init_Fixed(((iFixedLong) a.v << iFixedFracBits) / (iFixedLong) b.v);
}

iLocalDef void divv_Fixed(iFixed *a, const iFixed b) {
    *a = div_Fixed(*a, b);
}

iLocalDef iFixed divi_Fixed(const iFixed a, int i) {
    return div_Fixed(a, initi_Fixed(i));
}

iLocalDef iFixed divf_Fixed(const iFixed a, float f) {
    return div_Fixed(a, initf_Fixed(f));
}

iLocalDef iFixed64 value_Fixed(const iFixed a) {
    return a.v;
}

iLocalDef int64_t i64_Fixed(const iFixed a) {
    return a.v >> iFixedFracBits;
}

iLocalDef int32_t i32_Fixed(const iFixed a) {
    return (int32_t) a.v >> iFixedFracBits;
}

iLocalDef double f64_Fixed(const iFixed a) {
    return (double) a.v / iFixedUnit;
}

iLocalDef float f32_Fixed(const iFixed a) {
    return (float) f64_Fixed(a);
}

iLocalDef iFixed min_Fixed(const iFixed a, const iFixed b)  { return init_Fixed(iMin(a.v, b.v)); }
iLocalDef iFixed max_Fixed(const iFixed a, const iFixed b)  { return init_Fixed(iMax(a.v, b.v)); }
iLocalDef iFixed neg_Fixed(const iFixed a)                  { return init_Fixed(-a.v); }
iLocalDef iFixed abs_Fixed(const iFixed a)                  { if (a.comp.sign) return neg_Fixed(a); else return a; }

iLocalDef iFixed mix_Fixed(const iFixed a, const iFixed b, const iFixed t) {
    return add_Fixed(a, mul_Fixed(sub_Fixed(b, a), t));
}

iLocalDef iFixed random_Fixed(void) {
    return initf_Fixed(iRandomf());
}

//-------------------------------------------------------------------------------------------------

iDeclareType(Fixed2)

struct Impl_Fixed2 {
    iFixed x, y;
};

iLocalDef iFixed2 init1_X2(const iFixed x) {
    return (iFixed2){ x, x };
}

iLocalDef iFixed2 initi1_X2(int x) {
    return init1_X2(initi_Fixed(x));
}

iLocalDef iFixed2 initf1_X2(float x) {
    return init1_X2(initf_Fixed(x));
}

iLocalDef iFixed2 init_X2(const iFixed x, const iFixed y) {
    return (iFixed2){ x, y };
}

iLocalDef iFixed2 initi_X2(int x, int y) {
    return init_X2(initi_Fixed(x), initi_Fixed(y));
}

iLocalDef iFixed2 initf_X2(float x, float y) {
    return init_X2(initf_Fixed(x), initf_Fixed(y));
}

iLocalDef iFixed2 initv_X2(const iFixed *v) {
    return (iFixed2){ v[0], v[1] };
}

iLocalDef iFixed2 zero_X2(void) {
    return (iFixed2){ .x = { 0 }, .y = { 0 } };
}

iLocalDef iFixed2 one_X2(void) {
    return init1_X2(one_Fixed());
}

iLocalDef void store_X2(const iFixed2 d, iFixed *p_out) {
    p_out[0] = d.x;
    p_out[1] = d.y;
}

iLocalDef iFixed2 yx_X2(const iFixed2 d) {
    return (iFixed2){ d.y, d.x };
}

iLocalDef iFixed2 dx_X2(const iFixed2 a, const iFixed dx) { return (iFixed2){ add_Fixed(a.x, dx), a.y }; }
iLocalDef iFixed2 dy_X2(const iFixed2 a, const iFixed dy) { return (iFixed2){ a.x, add_Fixed(a.y, dy) }; }

iLocalDef iFixed2 add_X2      (const iFixed2 a, const iFixed2 b)  { return (iFixed2){ add_Fixed(a.x, b.x), add_Fixed(a.y, b.y) }; }
iLocalDef iFixed2 addi_X2     (const iFixed2 a, int b)            { return add_X2(a, initi_X2(b, b)); }
iLocalDef iFixed2 sub_X2      (const iFixed2 a, const iFixed2 b)  { return (iFixed2){ sub_Fixed(a.x, b.x), sub_Fixed(a.y, b.y) }; }
iLocalDef iFixed2 mul_X2      (const iFixed2 a, const iFixed2 b)  { return (iFixed2){ mul_Fixed(a.x, b.x), mul_Fixed(a.y, b.y) }; }
iLocalDef iFixed2 muli_X2     (const iFixed2 a, int b)            { return (iFixed2){ muli_Fixed(a.x, b), muli_Fixed(a.y, b) }; }
iLocalDef iFixed2 mulf_X2     (const iFixed2 a, float b)          { return (iFixed2){ mulf_Fixed(a.x, b), mulf_Fixed(a.y, b) }; }
iLocalDef iFixed2 div_X2      (const iFixed2 a, const iFixed2 b)  { return (iFixed2){ div_Fixed(a.x, b.x), div_Fixed(a.y, b.y) }; }
iLocalDef iFixed2 divi_X2     (const iFixed2 a, int b)            { return (iFixed2){ divi_Fixed(a.x, b), divi_Fixed(a.y, b) }; }
iLocalDef iFixed2 divf_X2     (const iFixed2 a, float b)          { return (iFixed2){ divf_Fixed(a.x, b), divf_Fixed(a.y, b) }; }

iLocalDef iFixed2 addv_X2     (iFixed2 *a, const iFixed2 b)  { addv_Fixed(&a->x, b.x); addv_Fixed(&a->y, b.y); return *a; }
iLocalDef iFixed2 addiv_X2    (iFixed2 *a, int b)            { addv_X2(a, initi_X2(b, b)); return *a; }
iLocalDef iFixed2 subv_X2     (iFixed2 *a, const iFixed2 b)  { subv_Fixed(&a->x, b.x); subv_Fixed(&a->y, b.y); return *a; }
iLocalDef iFixed2 mulv_X2     (iFixed2 *a, const iFixed2 b)  { mulv_Fixed(&a->x, b.x); mulv_Fixed(&a->y, b.y); return *a; }
iLocalDef iFixed2 muliv_X2    (iFixed2 *a, int b)            { mulv_Fixed(&a->x, initi_Fixed(b)); mulv_Fixed(&a->y, initi_Fixed(b)); return *a; }
iLocalDef iFixed2 mulfv_X2    (iFixed2 *a, float b)          { mulv_Fixed(&a->x, initf_Fixed(b)); mulv_Fixed(&a->y, initf_Fixed(b)); return *a; }
iLocalDef iFixed2 divv_X2     (iFixed2 *a, const iFixed2 b)  { divv_Fixed(&a->x, b.x); divv_Fixed(&a->y, b.y); return *a; }
iLocalDef iFixed2 diviv_X2    (iFixed2 *a, int b)            { divv_Fixed(&a->x, initi_Fixed(b)); divv_Fixed(&a->y, initi_Fixed(b)); return *a; }
iLocalDef iFixed2 divfv_X2    (iFixed2 *a, float b)          { divv_Fixed(&a->x, initf_Fixed(b)); divv_Fixed(&a->y, initf_Fixed(b)); return *a; }

iLocalDef iFixed2 min_X2      (const iFixed2 a, const iFixed2 b)  { return (iFixed2){ min_Fixed(a.x, b.x), min_Fixed(a.y, b.y) }; }
iLocalDef iFixed2 max_X2      (const iFixed2 a, const iFixed2 b)  { return (iFixed2){ max_Fixed(a.x, b.x), max_Fixed(a.y, b.y) }; }
iLocalDef iFixed2 neg_X2      (const iFixed2 a)                   { return (iFixed2){ neg_Fixed(a.x), neg_Fixed(a.y) }; }
iLocalDef iFixed2 abs_X2      (const iFixed2 a)                   { return (iFixed2){ abs_Fixed(a.x), abs_Fixed(a.y) }; }

iLocalDef iBool   isEqual_X2  (const iFixed2 a, const iFixed2 b)  { return a.x.v == b.x.v && a.y.v == b.y.v; }

iLocalDef iBoolv equal_X2   (const iFixed2 a, const iFixed2 b) {
    return init2_Boolv(a.x.v == b.x.v, a.y.v == b.y.v);
}

iLocalDef iBoolv notEqual_X2(const iFixed2 a, const iFixed2 b) {
    return init2_Boolv(a.x.v != b.x.v, a.y.v != b.y.v);
}

iLocalDef iBoolv greater_X2 (const iFixed2 a, const iFixed2 b) {
    return init2_Boolv(a.x.v > b.x.v, a.y.v > b.y.v);
}

iLocalDef iBoolv greaterEqual_X2 (const iFixed2 a, const iFixed2 b) {
    return init2_Boolv(a.x.v >= b.x.v, a.y.v >= b.y.v);
}

iLocalDef iBoolv less_X2 (const iFixed2 a, const iFixed2 b) {
    return init2_Boolv(a.x.v < b.x.v, a.y.v < b.y.v);
}

iLocalDef iBoolv lessEqual_X2 (const iFixed2 a, const iFixed2 b) {
    return init2_Boolv(a.x.v <= b.x.v, a.y.v <= b.y.v);
}

iLocalDef iFixed2   clamp_X2    (const iFixed2 t, const iFixed2 a, const iFixed2 b) { return min_X2(max_X2(t, a), b); }
iLocalDef iFixed    sum_X2      (const iFixed2 a) { return init_Fixed(a.x.v + a.y.v); }
iLocalDef iFixed    dot_X2      (const iFixed2 a, const iFixed2 b) { return sum_X2(mul_X2(a, b)); }
iLocalDef iFixed    lengthSq_X2 (const iFixed2 a) { return dot_X2(a, a); }
iLocalDef float     lengthf_X2  (const iFixed2 a) { return sqrtf(f32_Fixed(lengthSq_X2(a))); }
iLocalDef iFixed    length_X2   (const iFixed2 a) { return initf_Fixed(lengthf_X2(a)); }
iLocalDef iFixed2   normalize_X2(const iFixed2 a) { return div_X2(a, init1_X2(length_X2(a))); }

iLocalDef iFixed2 sqrt_X2(const iFixed2 a) {
    return (iFixed2){ initf_Fixed(sqrtf(f32_Fixed(a.x))),
                      initf_Fixed(sqrtf(f32_Fixed(a.y))) };
}

iLocalDef iFixed2 mix_X2(const iFixed2 a, const iFixed2 b, const iFixed t) {
    return add_X2(a, mul_X2(sub_X2(b, a), init1_X2(t)));
}

//-------------------------------------------------------------------------------------------------

iDeclareType(Fixed3)

struct Impl_Fixed3 {
    iFixed x, y, z;
};
