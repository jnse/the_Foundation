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

iVec2 random_Rect(const iRect *d) {
    return init_I2(iRandom(d->pos.x, right_Rect(d)), iRandom(d->pos.y, bottom_Rect(d)));
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

void expand_Rect(iRect *d, iVec2 value) {
    subv_I2(&d->pos, value);
    addv_I2(&d->size, muli_I2(value, 2));
}
