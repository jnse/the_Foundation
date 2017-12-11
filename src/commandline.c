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

iDefineClass(CommandLine)
iDefineObjectConstructionArgs(CommandLine, (int argc, char **argv), argc, argv)

static void loadArgumentsFile_CommandLine_(iCommandLine *d, const char *path) {
    //
#if 0
    void CommandLine::parse(String const &cmdLine)
    {
        String::const_iterator i = cmdLine.begin();

        // This is unset if we encounter a terminator token.
        bool isDone = false;

        // Are we currently inside quotes?
        bool quote = false;

        while (i != cmdLine.end() && !isDone)
        {
            // Skip initial whitespace.
            String::skipSpace(i, cmdLine.end());

            // Check for response files.
            bool isResponse = false;
            if (*i == '@')
            {
                isResponse = true;
                String::skipSpace(++i, cmdLine.end());
            }

            String word;

            while (i != cmdLine.end() && (quote || !(*i).isSpace()))
            {
                bool copyChar = true;
                if (!quote)
                {
                    // We're not inside quotes.
                    if (*i == '\"') // Quote begins.
                    {
                        quote = true;
                        copyChar = false;
                    }
                }
                else
                {
                    // We're inside quotes.
                    if (*i == '\"') // Quote ends.
                    {
                        if (i + 1 != cmdLine.end() && *(i + 1) == '\"') // Doubles?
                        {
                            // Normal processing, but output only one quote.
                            i++;
                        }
                        else
                        {
                            quote = false;
                            copyChar = false;
                        }
                    }
                }

                if (copyChar)
                {
                    word.push_back(*i);
                }

                i++;
            }

            // Word has been extracted, examine it.
            if (isResponse) // Response file?
            {
                parseResponseFile(word);
            }
            else if (word == "--") // End of arguments.
            {
                isDone = true;
            }
            else if (!word.empty()) // Make sure there *is *a word.
            {
                d->appendArg(word);
            }
        }
    }
#endif
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

iCommandLineArg *checkArgumentWithValues_CommandLine(const iCommandLine *d, const char *arg,
                                                     int minCount, int maxCount) {

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
