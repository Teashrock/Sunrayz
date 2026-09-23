// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#include "lexer.h"
#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

bool CharIsIgnored(char c) {
    const char ignored[] = {9, 10, 13, 32}; // Tab, whitespace, caret shift, and newline
    for (int i = 0; i < 4; i++) { // The 4 here is the length of ignored array
        if (c == ignored[i]) {
            return true;
        }
    }
    return false;
}

/// Called first in the process of parsing:
/// picks up the tokens from the code,
/// separating them from syntactic water;
/// takes opened file pointer as an argument
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
        if (onToken) {
            if (CharIsIgnored(*c)) {
                fseek(f, filePosition - tokenLength, SEEK_SET);
                currToken = (SzToken*)MemAlloc(sizeof(SzToken));
                currToken->token = (char*)MemAlloc(sizeof(char) * (tokenLength + 1));
                currToken->next = NULL;
                if (tree != NULL) {
                    tree = currToken;
                } else if (prevToken == NULL) {
                    prevToken = currToken;
                } else {
                    prevToken->next = currToken;
                }
                fread(currToken->token, sizeof(char), tokenLength + 1, f);
                prevToken = currToken;
                tokenLength = 0;
                onToken = false;
                fseek(f, filePosition, SEEK_SET);
            } else {
                tokenLength++;
            }
        } else {
            filePosition++;
            if (!CharIsIgnored(*c)) {
                onToken = true;
            }
        }
    }
    return tree;
}
