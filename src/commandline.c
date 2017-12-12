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
        i = skipSpace_CStr(i);
        // Check for a nested argument file.
        iBool isResponse = iFalse;
        if (*i == '@') {
            isResponse = iTrue;
            i = skipSpace_CStr(i + 1);
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
            loadArgumentsFile_CommandLine_(d, argv[i] + 1);
        }
        else {
            pushBackCStr_StringList(&d->args, argv[i]);
        }
    }
}

void deinit_CommandLine(iCommandLine *d) {
    deinit_StringList(&d->args);
}

static int cmpArg_(const iRangecc *entry, const iRangecc *arg) {
    iAssert(*arg->start != '-');
    iAssert(*entry->start != '-');
    if (size_Range(arg) == 1) {
        // Single-letter arguments can be joined in a longer entry.
        return findAscii_Rangecc(entry, *arg->start)? 0 : 1;
    }
    const char *eql = findAscii_Rangecc(entry, '=');
    const size_t entryLen = (eql? eql - entry->start : size_Range(entry));
    if (size_Range(arg) != entryLen) {
        return iCmp(size_Range(arg), entryLen);
    }
    return iCmpStrN(entry->start, arg->start, size_Range(arg));
}

static size_t findArg_CommandLine_(const iCommandLine *d, const iRangecc *arg) {
    iAssert(*arg->start != '-');
    iConstForEach(StringList, i, &d->args) {
        iRangecc entry = range_String(i.value);
        if (*entry.start != '-') continue;
        entry.start++;
        if (size_Range(arg) > 1) { // Long args must have two dashes.
            if (*entry.start != '-') continue;
            entry.start++;
        }
        else if (*entry.start == '-') {
            continue; // Want short, got long.
        }
        if (!cmpArg_(&entry, arg)) {
            return i.pos;
        }
    }
    return iInvalidPos;
}

iBool contains_CommandLine(const iCommandLine *d, const char *arg) {
    const iRangecc args = range_CStr(arg);
    iRangecc range = { NULL, NULL };
    while (nextSplit_Rangecc(&args, ";", &range)) {
        if (findArg_CommandLine_(d, &range) != iInvalidPos)
            return iTrue;
    }
    return iFalse;
}

static iCommandLineArg *checkArgumentWithValues_CommandLine_
    (const iCommandLine *d, const iRangecc *arg, int minCount, int maxCount) {
    const size_t pos = findArg_CommandLine_(d, arg);
    if (pos == iInvalidPos) return NULL;
    const iString *lineEntry = constAt_StringList(&d->args, pos);
    size_t equalPos;
    if ((equalPos = indexOf_String(lineEntry, '=')) != iInvalidPos) {
        if (minCount > 1 || maxCount == 0) return NULL;
        // There is a single value included in the entry.
        iCommandLineArg *clArg = new_CommandLineArg();
        clArg->pos = pos;
        set_String(&clArg->arg, lineEntry);
        pushBackCStr_StringList(&clArg->values, cstr_String(lineEntry) + equalPos + 1);
        truncate_String(&clArg->arg, equalPos);
        return clArg;
    }
    // Check how many values are provided.
    size_t endPos;
    for (endPos = pos + 1;
         endPos < size_StringList(&d->args) &&
             !startsWith_String(constAt_StringList(&d->args, endPos), "-");
         endPos++) {}
    int valueCount = (int) (endPos - pos - 1);
    if (valueCount < minCount) {
        return NULL;
    }
    if (maxCount != unlimitedValues_CommandLine) {
        valueCount = iMin(valueCount, maxCount);
    }
    iCommandLineArg *clArg = new_CommandLineArg();
    clArg->pos = pos;
    set_String(&clArg->arg, lineEntry);
    for (size_t i = pos + 1; i < endPos; ++i) {
        pushBack_StringList(&clArg->values, constAt_StringList(&d->args, i));
    }
    return clArg;
}

iCommandLineArg *checkArgumentWithValues_CommandLine
    (const iCommandLine *d, const char *arg, int minCount, int maxCount) {
    const iRangecc args = range_CStr(arg);
    iRangecc range = { NULL, NULL };
    while (nextSplit_Rangecc(&args, ";", &range)) {
        iCommandLineArg *clArg = checkArgumentWithValues_CommandLine_(d, &range, minCount, maxCount);
        if (clArg) return clArg;
    }
    return NULL;
}

//---------------------------------------------------------------------------------------

iDefineClass(CommandLineArg)
iDefineObjectConstruction(CommandLineArg)

void init_CommandLineArg(iCommandLineArg *d) {
    init_StringList(&d->values);
    init_String(&d->arg);
}

void deinit_CommandLineArg(iCommandLineArg *d) {
    deinit_String(&d->arg);
    deinit_StringList(&d->values);
}
