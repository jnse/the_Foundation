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

static void printv(const char *msg, iFloat4 v) {
    iFloatVec4 vec = values_F4(v);
    printf("%9s: ( %10f %10f %10f %10f )\n", msg, vec.v[0], vec.v[1], vec.v[2], vec.v[3]);
}

int main(int argc, char **argv) {
    init_CPlus();
    iUnused(argc, argv);
    /* Basic vectoring. */ {
        printv("zero", zero_F4());
        printv("init", init_F4(1, 2, 3, 4)); {
            float f[4];
            store_F4(init_F4(1, 2, 3, 4), f);
            printf("stored: %f %f %f %f\n", f[0], f[1], f[2], f[3]);
        }
        printv("zxy", zxy_F4(init_F4(1, 2, 3, 4)));
        iFloat4 a = init_F4(1.5, -2, 3.5, -4);
        iFloat4 b = init_F4(iRandomf(), iRandomf(), iRandomf(), 1);
        setW_F4(&b, 100.5f);
        printv("a", a);
        printv("neg(a)", neg_F4(a));
        printv("abs(a)", abs_F4(a));
        printv("b", b);
        printv("a+b", add_F4(a, b));
        printf("Length: %f\n", length_F3(init_F3(3, -3, 2)));
        for (int i = 0; i <= 10; ++i) {
            printf("%2i: ", i);
            printv("mix", mix_F4(a, b, i/10.f));
        }
    }
}
