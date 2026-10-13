// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#include "lexer.h"

#include "../base.h"
#include "../external/re.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((always_inline))
static inline bool CharIsIgnored(char c) {
    const char ignored[] = {9, 10, 13, 32}; // Tab, whitespace, caret shift, and newline
    for (int i = 0; i < 4; i++) { // The 4 here is the length of ignored array
        if (c == ignored[i]) {
            return true;
        }
    }
    return false;
}

__attribute__((always_inline))
static inline SzToken* CreateToken(void) {
    SzToken* ptr = (SzToken*)MemAlloc(sizeof(SzToken));
    *ptr = (SzToken){
        .token = NULL,
        .next = NULL
    };
    return ptr;
}

SzToken* Tokenise(const char* fileName) {
    FILE* f = fopen(fileName, "rt");
    SzToken* currToken = NULL;
    SzToken* prevToken = NULL;
    SzToken* tree = NULL;
    // Regex for script execution and string quote marks syntax
    re_t plRegex = re_compile("@(.*)\(");
    re_t stringRegex = re_compile("\"(.*)\"");
    int matchLength;
    char c[1];
    char* workedString;
    while (true) {
        int readResult = fread(c, sizeof(char), 1, f);
        if (readResult < 1) {
            if (feof(f)) {
                fclose(f);
                break;
            } else if (ferror(f)) {
                printf("Error reading file %s!\n", fileName);
                exit(1);
            }
        }
        if (!CharIsIgnored(*c)) {
            fseek(f, -1, SEEK_CUR);
            if (tree) { // Backing up current token pointer if tree exists
                prevToken = currToken;
            }
            currToken = CreateToken();
            if (!tree) { // Saving first token as the top of the tree
                tree = currToken;
            } else {
                prevToken->next = currToken; // Linking current token to the previous one
            }
            // Reading from the current position in file
            // until we meet a tab, a whitespace, a caret shift, or a newline
            fseek(f, StringReadUntilArr(f, (char[]){9, 10, 13, 32}, &workedString) + 1, SEEK_SET);
            // If we find a @xxx(-style pattern, we need to split the token to two
            if (re_matchp(plRegex, workedString, &matchLength) != -1) {
                char* tokenOne, tokenTwo;
                char* ptr = strstr(workedString, "(");
                strncpy(tokenOne, workedString, ptr - workedString);
                strncpy(tokenTwo, workedString, size_t n);
            }
        }
    }
    return tree;
}
