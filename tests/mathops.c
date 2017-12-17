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

static void printNum(float n) {
    if (n == 0.f) {
        printf("    .      ");
    }
    else {
        printf("%11f", n);
    }
}

static void printv(const char *msg, iFloat4 v) {
    const iFloatVec4 vec = values_F4(v);
    printf("%9s: ( ", msg);
    for (int i = 0; i < 4; ++i) printNum(vec.v[i]);
    printf(" )\n");
}

static void printv3(const char *msg, iFloat3 v) {
    const iFloatVec3 vec = values_F3(v);
    printf("%9s: ( ", msg);
    for (int i = 0; i < 3; ++i) printNum(vec.v[i]);
    printf(" )\n");
}

static void printMat(const char *msg, const iMat4 *m) {
    float vals[16];
    store_Mat4(m, vals);
    printf("%s: [", msg);
    for (int i = 0; i < 16; i += 4) {
        printf("\n    ");
        for (int j = 0; j < 4; ++j) printNum(vals[i+j]);
    }
    puts(" ]");
}

int main(int argc, char **argv) {
    init_CPlus();
    iUnused(argc, argv);
    /* Basic vectoring. */ {
        printv("zero", zero_F4());
        printv("init4", init_F4(1, 2, 3, 4)); {
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
        printf("Length: %f %f %f\n",
               length_F3(init_F3(-2, 0, 0)),
               length_F3(init_F3(0, -2, 0)),
               length_F3(init_F3(0, 0, -2)));
        printv3("init3", init_F3(2, 3, 4)); {
            iFloat3 v3 = init_F3(1, 1, 1);
            setX_F3(&v3, 2);
            printv3("set x", v3);
            setY_F3(&v3, 3);
            printv3("set y", v3);
            setZ_F3(&v3, 4);
            printv3("set z", v3);
        }
        for (int i = 0; i <= 10; ++i) {
            printf("%2i: ", i);
            printv("mix", mix_F4(a, b, i/10.f));
        }
    }
    /* Matrices. */ {
        printf("dot3: %f\n", dot_F3(init_F3(1, 2, 3), init_F3(3, 4, 2)));

        iMat4 mat, b, c;
        init_Mat4(&mat);
        printMat("identity", &mat);
        initScale_Mat4(&b, init1_F3(3));
        printMat("3x", &b);
        mul_Mat4(&mat, &b);
        printMat("mult", &mat);
        copy_Mat4(&c, &mat);
        scalef_Mat4(&c, .1f);
        printMat("scaled", &c);

        iMat4 s;
        initScale_Mat4(&s, init_F3(4, 3, 2));
        printMat("scale", &s);

        printv("vec mul", mulF4_Mat4(&s, init1_F4(1)));

        iMat4 t, u;
        initTranslate_Mat4(&t, init_F3(1, 2, 3));
        printMat("xlat", &t);
        //printv3("vec trl", mulF3_Mat4(&t, init_F3(14, 13, 12)));
        printv3("vec trl", mulF3_Mat4(&t, init_F3(iRandomf(), iRandomf(), iRandomf())));

        copy_Mat4(&u, &t);
        mul_Mat4(&u, &s);
        mul_Mat4(&s, &t);
        printMat("s*t", &s);
        printMat("t*s", &u);

        iMat4 s_t; copy_Mat4(&s_t, &s);
        iMat4 t_s; copy_Mat4(&t_s, &u);

        printv3("s_t * (1,1,1)", mulF3_Mat4(&s_t, init_F3(1, 1, 1)));
        printv3("t_s * (1,1,1)", mulF3_Mat4(&t_s, init_F3(1, 1, 1)));
    }
}
