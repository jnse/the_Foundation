/**
@authors Copyright (c) 2017 Jaakko Keränen <jaakko.keranen@iki.fi>

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

#include <c_plus/defs.h>
#include <c_plus/math.h>

static void printv(iFloat4 v) {
    iFloatVec4 vec = values_Float4(v);
    printf("(%10f %10f %10f %10f)\n", vec.v[0], vec.v[1], vec.v[2], vec.v[3]);
}

int main(int argc, char **argv) {
    init_CPlus();
    iUnused(argc, argv);
    /* Basic vectoring. */ {
        printv(zero_Float4());
        printv(init_Float4(1, 2, 3, 4));
        printv(zxy_Float4(init_Float4(1, 2, 3, 4)));
        iFloat4 a = init_Float4(1.5, 2, 3.5, 4);
        iFloat4 b = init_Float4(iRandomf(), iRandomf(), iRandomf(), 1);
        printv(add_Float4(a, b));
        printf("Length: %f\n", length_Float3(init_Float3(3, -3, 2)));
        for (int i = 0; i <= 10; ++i) {
            printf("%i: ", i);
            printv(mix_Float4(a, b, i/10.f));
        }
    }
}
