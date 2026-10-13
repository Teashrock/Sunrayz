// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#include "lexer.h"

#include "../base.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool CharIsIgnored(char c) {
    const char ignored[] = {9, 10, 13, 32}; // Tab, whitespace, caret shift, and newline
    for (int i = 0; i < 4; i++) { // The 4 here is the length of ignored array
        if (c == ignored[i]) {
            return true;
        }
    }
    return false;
}

SzToken* CreateToken(void) {
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
    char c[1];
    size_t tokenLength = 0;
    long filePosition = ftell(f);
    bool onToken = false;
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
            fseek(f, StringReadUntilArr(f, (char[]){9, 10, 13, 32}, &currToken->token) + 1, SEEK_SET);
        }
    }
    return tree;
}
