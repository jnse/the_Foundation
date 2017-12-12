#pragma once

/** @file c_plus/commandline.h  Command line options parsing.

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

#include "stringlist.h"
#include "stringhash.h"

iDeclareClass(CommandLine)

iDeclareType(CommandLine)
iDeclareType(CommandLineArg)

struct Impl_CommandLine {
    iStringList args;
    iStringHash *defined;
};

enum { unlimitedValues_CommandLine = -1 };

iDeclareObjectConstructionArgs(CommandLine, int argc, char **argv)

void        init_CommandLine        (iCommandLine *, int argc, char **argv);
void        deinit_CommandLine      (iCommandLine *);

void        defineValues_CommandLine  (iCommandLine *, const char *arg, int valueCount);
void        defineValuesN_CommandLine (iCommandLine *, const char *arg, int minCount, int maxCount);

iBool       contains_CommandLine    (const iCommandLine *, const char *arg);

iCommandLineArg *checkArgument_CommandLine(const iCommandLine *d, const char *arg);

/**
 * Finds an argument and its values from the command line.
 *
 * @param arg       Argument(s) to find. This can be a single argument or a list of
 *                  semicolon-separated arguments. The check is done in the specified
 *                  order.
 * @param minCount  Minimum number of values.
 * @param maxCount  Maximum number of values, or unlimitedValues_CommandLine.
 *
 * @return New CommandLineArg object with the found argument and values, or @c NULL.
 */
iCommandLineArg *checkArgumentValuesN_CommandLine  (const iCommandLine *, const char *arg,
                                                   int minCount, int maxCount);

static inline const iStringList *args_CommandLine(const iCommandLine *d) {
    return &d->args;
}

static inline const iString *at_CommandLine(const iCommandLine *d, size_t pos) {
    return constAt_StringList(&d->args, pos);
}

static inline iCommandLineArg *checkArgumentValues_CommandLine(const iCommandLine *d, const char *arg, int count) {
    return checkArgumentValuesN_CommandLine(d, arg, count, count);
}

iDeclareConstIterator(CommandLine, const iCommandLine *)

struct IteratorImpl_CommandLine {

    const iCommandLine *cmdLine;
};

//---------------------------------------------------------------------------------------

struct Impl_CommandLineArg {
    iStringList values;
    iString arg;
    size_t pos;
};

iDeclareClass(CommandLineArg)
iDeclareObjectConstruction(CommandLineArg)

void        init_CommandLineArg     (iCommandLineArg *);
void        deinit_CommandLineArg   (iCommandLineArg *);

static inline const iString *value_CommandLineArg(const iCommandLineArg *d, size_t pos) {
    return constAt_StringList(&d->values, pos);
}

static inline const iStringList *values_CommandLineArg(const iCommandLineArg *d) {
    return &d->values;
}
