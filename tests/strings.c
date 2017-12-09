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

#include <c_plus/file.h>
#include <c_plus/math.h>
#include <c_plus/stringarray.h>
#include <c_plus/stringlist.h>

int main(int argc, char *argv[]) {
    iUnused(argc, argv);
    init_CPlus();
    /* Test Unicode strings. */ {
        iString *s = collect_String(newCStr_String("A_Äö\U0001f698a"));
        printf("String: %s length: %zu size: %zu\n", cstr_String(s), length_String(s), size_String(s)); {
            iConstForEach(String, i, s) {
                printf(" char: %04x [%lc]\n", i.value, i.value);
            }
        }
        printf("Backwards:\n"); {
            iReverseConstForEach(String, i , s) {
                printf(" char: %04x [%lc]\n", i.value, i.value);
            }
        }
        printf("Starts with: %i %i\n", startsWith_String(s, "a"), startsWithCase_String(s, "a"));
        printf("Ends with: %i %i\n", endsWith_String(s, "a"), endsWithCase_String(s, "A"));
        printf("Mid: %s\n", cstr_String(collect_String(mid_String(s, 3, 1))));
        printf("ö is at: %zu %zu\n", indexOfCStr_String(s, "ö"), indexOf_String(s, u'ö'));
        truncate_String(s, 3);
        printf("Truncated: %s\n", cstr_String(s));
    }
    /* Test an array of strings. */ {
        iStringArray *sar = newStringsCStr_StringArray("Hello World", "Another string", "3rd text", NULL);
        printf("StringArray contents:\n");
        iConstForEach(StringArray, i, sar) {
            printf("%4zu: \"%s\"\n", index_StringArrayConstIterator(&i), cstr_String(*i.value));
        }
        iRelease(sar);
    }
    /* Test a list of strings. */ {
        iFile *file = newCStr_File("/Users/jaakko/src/cplus/CMakeLists.txt");
        printf("Contents of %s:\n", cstr_String(path_File(file)));
        if (open_File(file, text_FileMode)) {
            iStringList *list = readLines_File(file);
            iConstForEach(StringList, i, list) {
                printf("%4zu: \"%s\"\n", i.pos, cstr_String(i.value));
            }
            size_t n = size_StringList(list);
            for (size_t k = 0; k < n*4/5; ++k) {
                remove_StringList(list, iRandom(0, (int) size_StringList(list)));
            }
            iConstForEach(StringList, j, list) {
                printf("%4zu: \"%s\"\n", j.pos, cstr_String(j.value));
            }
            iRelease(list);
        }
        iRelease(file);
    }
}
