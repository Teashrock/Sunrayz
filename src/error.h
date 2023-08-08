#pragma once

#include <string.h>

typedef enum {
    OK,
    GENERIC,
    NOTFOUND,
    DEALLOCATION,
} SzError;

#define CREATE_STR(VAR, STR) \
    char* VAR = (char*)MemAlloc((strlen(STR) * sizeof(char*)) + 1); \
    strcpy(VAR, STR);

void SzRuntimeError(char* title, char* text);
void SzDebugInfo(char* text, bool* oneshot);
