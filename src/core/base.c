#include "base.h"

/// Reads a string from an opened file
/// up to the provided character (char until),
/// then writes it by a provided char* pointer (dest).
/// No need to allocate dest in advance.
long StringReadUntil(FILE* f, char until, char** dest) {
    // Counting chars until the specific symbol is met
    char c[1];
    int startPosition = ftell(f);
    int charCount = 0;
    while (true) {
        fread(c, sizeof(char), 1, f);
        if (c[0] == until) {
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

/// Checks if a string can be seamlessly converted into an integer.
/// Warning: expects a zstring.
bool StringIsInteger(char* string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            return false;
    }
    return true;
}

/// Checks if a string can be seamlessly converted into a boolean.
/// Warning: expects a zstring.
bool StringIsBool(char* string) {
    char* tokenTrue = "true";
    char* tokenFalse = "false";
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] != tokenTrue[i] || string[i] != tokenFalse[i])
            return false;
    }
    return true;
}
