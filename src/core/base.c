// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#include "base.h"
#include <string.h>

/// Reads a string from an opened file
/// up to the provided character (char until),
/// then writes it by a provided char* pointer (dest).
/// Returns file position on the null char of the string.
/// No need to allocate dest in advance.
long StringReadUntil(FILE* f, char until, char** dest) {
    // Counting chars until the specific symbol is met
    char c[1];
    int startPosition = ftell(f);
    int charCount = 0;
    while (true) {
        fread(c, sizeof(char), 1, f);
        if (*c == until) {
            fseek(f, startPosition, SEEK_SET);
            break;
        }
        charCount++;
    }
    // Writing the string found into the destination
    *dest = (char*)MemAlloc(sizeof(char) * (charCount + 1));
    fread(*dest, sizeof(char), charCount, f);
    (*dest)[charCount + 1] = '\0';
    return ftell(f);
}

/// Reads a string from an opened file
/// up to a character in the provided string (char* until),
/// then writes it by a provided char* pointer (dest).
/// Returns file position on the null char of the string.
/// No need to allocate dest in advance.
long StringReadUntilArr(FILE* f, const char* const until, char** dest) {
    // Counting chars until the specific symbol is met
    char c[1];
    int startPosition = ftell(f);
    int charCount = 0;
    bool write = true;
    while (write) {
        fread(c, sizeof(char), 1, f);
        for (int i = 0; until[i] != '\0'; i++) {
            if (*c == until[i]) {
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

/// Checks if a string can be seamlessly converted into an integer.
/// Warning: expects a zstring.
bool StringIsInteger(const char* const string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            return false;
    }
    return true;
}

/// Checks if a string can be seamlessly converted into a float or double.
/// Warning: expects a zstring.
bool StringIsFraction(const char* const string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            if (!(string[i] == '.' && strchr(string, '.') == strrchr(string, '.')))
                return false;
    }
    return true;
}

/// Checks if a string can be seamlessly converted into a boolean.
/// Warning: expects a zstring.
bool StringIsBool(const char* const string) {
    char* tokenTrue = "true";
    char* tokenFalse = "false";
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] != tokenTrue[i] || string[i] != tokenFalse[i])
            return false;
    }
    return true;
}
