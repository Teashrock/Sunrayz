// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#include "base.h"
#include <stdio.h>
#include <string.h>

long StringReadUntil(FILE* f, char until, char** dest) {
    // Counting chars until the specific symbol is met
    int readResult;
    char c[1];
    int startPosition = ftell(f);
    int charCount = 0;
    while (true) {
        readResult = fread(c, sizeof(char), 1, f);
        if (*c == until || (readResult < 1 && feof(f))) {
            fseek(f, startPosition, SEEK_SET);
            break;
        }
        charCount++;
    }
    // Writing the string found into the destination
    *dest = (char*)MemAlloc(sizeof(char) * (charCount + 1));
    fread(*dest, sizeof(char), charCount, f);
    (*dest)[charCount] = '\0';
    return ftell(f);
}

long StringReadUntilArr(FILE* f, const char* const until, char** dest) {
    // Counting chars until the specific symbol is met
    int readResult;
    char c[1];
    int startPosition = ftell(f);
    int charCount = 0;
    bool write = true;
    while (write) {
        readResult = fread(c, sizeof(char), 1, f);
        for (int i = 0; until[i] != '\0'; i++) {
            if (*c == until[i] || (readResult < 1 && feof(f))) {
                write = false;
            }
        }
        charCount++;
    }
    fseek(f, startPosition, SEEK_SET);
    // Writing the string found into the destination
    *dest = (char*)MemAlloc(sizeof(char) * (charCount));
    fread(*dest, sizeof(char), charCount - 1, f);
    (*dest)[charCount] = '\0';
    return ftell(f);
}

bool StringIsInteger(const char* const string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            return false;
    }
    return true;
}

bool StringIsFraction(const char* const string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            if (!(string[i] == '.' && strchr(string, '.') == strrchr(string, '.')))
                return false;
    }
    return true;
}

bool StringIsBool(const char* const string) {
    if (strcmp(string, "true") && strcmp(string, "false"))
        return false;
    return true;
}
