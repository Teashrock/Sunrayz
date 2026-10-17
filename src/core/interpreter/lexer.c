// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#include "lexer.h"

#include "../base.h"
#include "../external/re.h"
#include "raylib.h"

#include <stdbool.h>
#include <stddef.h>
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

void CreateMultipleTokens(
    int idx,
    int tokenArrl,
    char* tokenArrv[],
    size_t workedStringLength,
    char* workedString,
    SzToken** currToken,
    SzToken** prevToken
) {
    switch (tokenArrl) {
        case 3:
        tokenArrv[0] = (char*)MemAlloc(sizeof(char) * idx + 1);
        tokenArrv[2] = (char*)MemAlloc(sizeof(char) * workedStringLength - idx - 1);
        strncpy(tokenArrv[0], workedString, idx);
        strncpy(tokenArrv[2], workedString, idx + 1);
        printf("----\nworkedString|%s\nidx|%i\ntokenArr[0]|%s\ntokenArr[2]|%s\n----\n", workedString, idx, tokenArrv[0], tokenArrv[2]);
        break;
        default:
        tokenArrv[1] = (char*)MemAlloc(sizeof(char) * workedStringLength);
        strncpy(tokenArrv[1], workedString + 1, workedStringLength - 1);
        printf("----\nworkedString|%s\ntokenArr[1]|%s\n----\n", workedString, tokenArrv[1]);
    }
    for (int i = 0; i < tokenArrl; i++) {
        if (tokenArrv[i] != NULL) {
            *currToken = CreateToken();
            (*currToken)->token = tokenArrv[i];
            (*prevToken)->next = *currToken;
            *prevToken = *currToken;
        }
    }
}

SzToken* Tokenise(const char* fileName) {
    FILE* f = fopen(fileName, "rt");
    SzToken* currToken = NULL;
    SzToken* prevToken = NULL;
    SzToken* tree = NULL;
    // Regex for various syntax
    re_t plRegex = re_compile("@(.*)\("); // Script execution
    re_t stringRegex = re_compile("\"(.*)\""); // String quote marks
    re_t colonRegex = re_compile(":"); // Colon
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
            // Reading from the current position in file
            // until we meet a tab, a whitespace, a caret shift, or a newline
            fseek(f, StringReadUntilArr(f, (char[]){9, 10, 13, 32}, &workedString) + 1, SEEK_SET);

            if (strcmp(workedString, ":")) {
                int idx; // idx == -1 is when it's not needed; TODO: refactor this later
                size_t workedStringLength = strlen(workedString);
                if (workedString[0] == ':') {
                    CreateMultipleTokens(-1, 2, (char*[]){":", NULL}, workedStringLength, workedString, &currToken, &prevToken);
                } else if (workedString[workedStringLength - 1] == ':') {
                    CreateMultipleTokens(-1, 2, (char*[]){NULL, ":"}, workedStringLength, workedString, &currToken, &prevToken);
                } else if ((idx = re_matchp(colonRegex, workedString, &matchLength)) != -1) {
                    CreateMultipleTokens(idx, 3, (char*[]){NULL, ":", NULL}, workedStringLength, workedString, &currToken, &prevToken);
                } else {
                    goto singleToken;
                }
            } else {
                singleToken:
                currToken = CreateToken();
                currToken->token = workedString;
                workedString = NULL;
            }
            if (!tree) { // Saving first token as the top of the tree
                tree = currToken;
            } else {
                prevToken->next = currToken; // Linking current token to the previous one
            }
            // If we find a @xxx(-style pattern, we need to split the token to two
            /*if (re_matchp(plRegex, workedString, &matchLength) != -1) {
                char* tokenOne, tokenTwo;
                char* ptr = strstr(workedString, "(");
                strncpy(tokenOne, workedString, ptr - workedString);
                strncpy(tokenTwo, workedString, size_t n);
            }*/
        }
    }
    return tree;
}
