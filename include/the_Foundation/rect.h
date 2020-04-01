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
    iInt2 pos;
    iInt2 size;
};

iDeclareType(Stream)
iDeclareTypeSerialization(Rect)

iLocalDef iRect zero_Rect(void) { return (iRect){ zero_I2(), zero_I2() }; }

iLocalDef iRect init_Rect(int x, int y, int width, int height) {
    return (iRect){ init_I2(x, y), init_I2(width, height) };
}

iLocalDef iRect initv_Rect(const int *v) {
    return (iRect){ initv_I2(v), initv_I2(v + 2) };
}

iLocalDef iRect initCorners_Rect(const iInt2 topLeft, const iInt2 bottomRight) {
    return (iRect){ topLeft, sub_I2(bottomRight, topLeft) };
}

iLocalDef iRect initCentered_Rect(const iInt2 center, const iInt2 size) {
    return (iRect){ sub_I2(center, divi_I2(size, 2)), size };
}

iLocalDef iRect initSize_Rect(int width, int height) {
    return init_Rect(0, 0, width, height);
}

iLocalDef int   left_Rect   (const iRect *d) { return d->pos.x; }
iLocalDef int   right_Rect  (const iRect *d) { return d->pos.x + d->size.x; }
iLocalDef int   top_Rect    (const iRect *d) { return d->pos.y; }
iLocalDef int   bottom_Rect (const iRect *d) { return d->pos.y + d->size.y; }
iLocalDef iInt2 mid_Rect    (const iRect *d) { return add_I2(d->pos, divi_I2(d->size, 2)); }

iLocalDef iInt2 topLeft_Rect(const iRect *d) { return d->pos; }

iLocalDef iInt2 topMid_Rect(const iRect *d) {
    return init_I2(d->pos.x + d->size.x / 2, d->pos.y);
}

iLocalDef iInt2 topRight_Rect(const iRect *d) {
    return init_I2(right_Rect(d), d->pos.y);
}

iLocalDef iInt2 bottomLeft_Rect(const iRect *d) {
    return init_I2(d->pos.x, bottom_Rect(d));
}

iLocalDef iInt2 bottomMid_Rect(const iRect *d) {
    return init_I2(d->pos.x + d->size.x / 2, bottom_Rect(d));
}

iLocalDef iInt2 bottomRight_Rect(const iRect *d) {
    return add_I2(d->pos, d->size);
}

iLocalDef iBool contains_Rect(const iRect *d, const iInt2 pos) {
    const iInt2 br = bottomRight_Rect(d);
    return pos.x >= d->pos.x && pos.y >= d->pos.y && pos.x < br.x && pos.y < br.y;
}

iBool   containsRect_Rect   (const iRect *, const iRect *other);
iBool   isOverlapping_Rect  (const iRect *, const iRect *other);

iLocalDef iBool isEmpty_Rect(const iRect *d) {
    return prod_I2(d->size) == 0;
}

iLocalDef iBool equal_Rect(const iRect *d, const iRect *other) {
    return all_Boolv2(equal_I2(d->pos, other->pos)) && all_Boolv2(equal_I2(d->size, other->size));
}

iRect   union_Rect          (const iRect *d, const iRect *other);
iRect   intersect_Rect      (const iRect *d, const iRect *other);
void    expand_Rect         (iRect *, iInt2 value);
void    adjustEdges_Rect    (iRect *, int top, int right, int bottom, int left);
iInt2   random_Rect         (const iRect *d);
iInt2   edgePos_Rect        (const iRect *d, int pos);
iInt2   randomEdgePos_Rect  (const iRect *d); // not a corner

iLocalDef void shrink_Rect  (iRect *d, iInt2 value) { expand_Rect(d, neg_I2(value)); }

iLocalDef iRect expanded_Rect(iRect d, iInt2 value) {
    expand_Rect(&d, value);
    return d;
}

iLocalDef iRect shrunk_Rect(iRect d, iInt2 value) {
    expand_Rect(&d, neg_I2(value));
    return d;
}

iLocalDef iRect adjusted_Rect(const iRect *d, iInt2 topLeft, iInt2 bottomRight) {
    return initCorners_Rect(add_I2(d->pos, topLeft), add_I2(bottomRight_Rect(d), bottomRight));
}

iLocalDef iRect moved_Rect(iRect d, iInt2 offset) {
    return (iRect){ add_I2(d.pos, offset), d.size };
}

iDeclareConstIterator(Rect, const iRect *)

struct ConstIteratorImpl_Rect {
    iBool value; // position is valid
    iInt2 pos;
    const iRect *rect;
};

#define iForRectRadius(iter, center, radius, body) { \
    const iInt2 center_ForRadius_ = (center); \
    const int radius_ForRadius_ = (radius); \
    const iRect rect_ForRadius_ = initCentered_Rect(center_ForRadius_, init1_I2(2 * radius_ForRadius_ + 1)); \
    iConstForEach(Rect, iter, &rect_ForRadius_) { \
        if (dist_I2(center_ForRadius_, iter.pos) + .5f <= radius_ForRadius_) { body } \
    } \
}

iEndPublic
