/** @file rect.c 2D integer rectangle.

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

#include "the_Foundation/rect.h"
#include "the_Foundation/math.h"

iBool containsRect_Rect(const iRect *d, const iRect *other) {
    const iVec2 br = sub_I2(bottomRight_Rect(other), one_I2());
    return contains_Rect(d, topLeft_Rect(other)) &&
           contains_Rect(d, init_I2(br.x, top_Rect(other))) && contains_Rect(d, br) &&
           contains_Rect(d, init_I2(left_Rect(other), br.y));
}

iBool isOverlapping_Rect(const iRect *d, const iRect *other) {
    /* Overlaps unless any one of the edges makes it impossible. */
    return !(other->pos.x >= right_Rect(d) || other->pos.y >= bottom_Rect(d) ||
             right_Rect(other) <= d->pos.x || bottom_Rect(other) <= d->pos.y);
}

iVec2 random_Rect(const iRect *d) {
    return add_I2(d->pos, random_I2(d->size));
}

static inline int edgeLength_Rect_(const iRect *d) {
    return 2 * (d->size.x - 1) + 2 * (d->size.y - 1);
}

iVec2 edgePos_Rect(const iRect *d, int pos) {
    if (pos < d->size.x) {
        return init_I2(d->pos.x + pos, d->pos.y);
    }
    pos -= d->size.x - 1;
    if (pos < d->size.y) {
        return init_I2(right_Rect(d) - 1, top_Rect(d) + pos);
    }
    pos -= d->size.y - 1;
    if (pos < d->size.x) {
        return init_I2(right_Rect(d) - 1 - pos, bottom_Rect(d) - 1);
    }
    pos -= d->size.x - 1;
    return init_I2(d->pos.x, bottom_Rect(d) - 1 - pos);
}

iVec2 randomEdgePos_Rect(const iRect *d) {
    const iVec2 dim = sub_I2(d->size, one_I2());
    int i;
    for (;;) {
        i = iRandom(1, edgeLength_Rect_(d));
        /* Do not pick one of the corners. */
        if (i != dim.x && i != dim.x + dim.y && i != 2 * dim.x + dim.y) {
            break;
        }
    }
    return edgePos_Rect(d, i);
}

void init_RectConstIterator(iRectConstIterator *d, const iRect *rect) {
    d->rect = rect;
    d->pos = rect->pos;
    d->value = !isEmpty_Rect(rect);
}

void next_RectConstIterator(iRectConstIterator *d) {
    d->pos.x++;
    if (d->pos.x == right_Rect(d->rect)) {
        d->pos.x = left_Rect(d->rect);
        d->pos.y++;
    }
    d->value = (d->pos.y < bottom_Rect(d->rect));
}

iRect union_Rect(const iRect *d, const iRect *other) {
    if (isEmpty_Rect(d)) {
        return *other;
    }
    if (isEmpty_Rect(other)) {
        return *d;
    }
    const iVec2 tl = min_I2(d->pos, other->pos);
    const iVec2 br = max_I2(bottomRight_Rect(d), bottomRight_Rect(other));
    return (iRect){ tl, sub_I2(br, tl) };
}

iRect intersect_Rect(const iRect *d, const iRect *other) {
    if (!isOverlapping_Rect(d, other)) {
        return zero_Rect();
    }
    const iVec2 tl = max_I2(d->pos, other->pos);
    const iVec2 br = min_I2(bottomRight_Rect(d), bottomRight_Rect(other));
    return (iRect){ tl, sub_I2(br, tl) };
}

void expand_Rect(iRect *d, iVec2 value) {
    subv_I2(&d->pos, value);
    addv_I2(&d->size, muli_I2(value, 2));
}

void adjustEdges_Rect(iRect *d, int top, int right, int bottom, int left) {
    d->pos.y += top;
    d->size.y -= top;
    d->size.y += bottom;
    d->pos.x += left;
    d->size.x -= left;
    d->size.x += right;
}
