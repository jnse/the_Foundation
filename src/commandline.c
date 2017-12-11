/** @file commandline.c  Command line options parser.

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

#include "c_plus/commandline.h"
#include "c_plus/file.h"
#include "c_plus/block.h"

iDefineClass(CommandLine)
iDefineObjectConstructionArgs(CommandLine, (int argc, char **argv), argc, argv)

static void loadArgumentsFile_CommandLine_(iCommandLine *d, const char *path) {
    iBeginCollect();
    const iString *input;
    /* Read the argument file contents. */ {
        iFile *file = iClob(newCStr_File(path));
        if (!open_File(file, readOnly_FileMode | text_FileMode)) {
            iEndCollect();
            return;
        }
        input = collect_String(readString_File(file));
        close_File(file);
    }
    // Parse into separate arguments.
    const char *i = cstr_String(input);
    iBool isDone = iFalse;
    iBool inQuote = iFalse;
    iBlock *word = collect_Block(new_Block(0));
    while (i != constEnd_String(input) && !isDone) {
        // Skip initial whitespace.
        i = skipSpace_String(i);
        // Check for a nested argument file.
        iBool isResponse = iFalse;
        if (*i == '@') {
            isResponse = iTrue;
            i = skipSpace_String(i + 1);
        }
        clear_Block(word);
        while (*i && (inQuote || !isspace(*i))) {
            iBool copyChar = iTrue;
            if (!inQuote) {
                if (*i == '"') {
                    inQuote = iTrue;
                    copyChar = iFalse;
                }
            }
            else { // Inside quotes.
                if (*i == '"') { // Quote ends.
                    if (i[1] == '"') { // Doubled?
                        // Normal processing, but output only one quote.
                        i++;
                    }
                    else {
                        inQuote = iFalse;
                        copyChar = iFalse;
                    }
                }
            }
            if (copyChar) pushBack_Block(word, *i);
            i++;
        }
        // Word has been extracted, examine it.
        if (isResponse) {
            loadArgumentsFile_CommandLine_(d, constData_Block(word));
        }
        else if (!cmpCStr_Block(word, "--")) { // End of arguments.
            isDone = iTrue;
        }
        else if (!isEmpty_Block(word)) {
            pushBackCStr_StringList(&d->args, constData_Block(word));
        }
    }
    iEndCollect();
}

void init_CommandLine(iCommandLine *d, int argc, char **argv) {
    init_StringList(&d->args);
    for (int i = 0; i < argc; ++i) {
        // Load response files.
        if (!iCmpStrN(argv[i], "@", 1)) {
            loadArgumentsFile_CommandLine_(d, argv[1] + 1);
        }
        else {
            pushBackCStr_StringList(&d->args, argv[i]);
        }
    }
}

void deinit_CommandLine(iCommandLine *d) {
    deinit_StringList(&d->args);
}

iBool contains_CommandLine(const iCommandLine *d, const char *arg) {

}

iCommandLineArg *checkArgumentWithValues_CommandLine
    (const iCommandLine *d, const char *arg, int minCount, int maxCount) {

}

//---------------------------------------------------------------------------------------

iDefineClass(CommandLineArg)
iDefineObjectConstruction(CommandLineArg)

void init_CommandLineArg(iCommandLineArg *d) {
    init_StringList(&d->values);
    init_String(&d->option);
}

void deinit_CommandLineArg(iCommandLineArg *d) {
    deinit_String(&d->option);
    deinit_StringList(&d->values);
}
