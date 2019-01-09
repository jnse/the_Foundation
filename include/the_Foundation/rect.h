#pragma once

/** @file the_Foundation/rect.h  2D integer rectangle.

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

#include "vec2.h"

iDeclareType(Rect)

iBeginPublic

struct Impl_Rect {
    iVec2 pos;
    iVec2 size;
};

static inline iRect init_Rect(int x, int y, int width, int height) {
    return (iRect){ init_I2(x, y), init_I2(width, height) };
}

static inline iRect initv_Rect(const int *v) {
    return (iRect){ initv_I2(v), initv_I2(v + 2) };
}

static inline int   left_Rect   (const iRect *d) { return d->pos.x; }
static inline int   right_Rect  (const iRect *d) { return d->pos.x + d->size.x; }
static inline int   top_Rect    (const iRect *d) { return d->pos.y; }
static inline int   bottom_Rect (const iRect *d) { return d->pos.y + d->size.y; }

static inline iVec2 bottomRight_Rect(const iRect *d) {
    return add_I2(d->pos, d->size);
}

static inline iBool contains_Rect(const iRect *d, const iVec2 pos) {
    const iVec2 br = bottomRight_Rect(d);
    return pos.x >= d->pos.x && pos.y >= d->pos.y && pos.x < br.x && pos.y < br.y;
}

static inline iBool isEmpty_Rect(const iRect *d) {
    return prod_I2(d->size) == 0;
}

static inline iBool equal_Rect(const iRect *d, const iRect *other) {
    return equal_I2(d->pos, other->pos) && equal_I2(d->size, other->size);
}

static inline iRect union_Rect(const iRect *d, const iRect *other) {
    const iVec2 br = min_I2(bottomRight_Rect(d), bottomRight_Rect(other));
    const iVec2 tl = max_I2(d->pos, other->pos);
    iRect u = { tl, sub_I2(br, tl) };
    if (u.size.x < 0 || u.size.y < 0) {
        u.size = zero_I2();
    }
    return u;
}

iVec2   random_Rect (const iRect *d);

iDeclareConstIterator(Rect, const iRect *)

struct ConstIteratorImpl_Rect {
    iBool value; // position is valid
    iVec2 pos;
    const iRect *rect;
};

iEndPublic
