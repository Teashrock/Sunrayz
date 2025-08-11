#pragma once

#include <string.h>
#include <raylib.h>

typedef enum {
    OK,
    GENERIC,
    NOTFOUND,
    DEALLOCATION,
} SzError;

#define CREATE_STR(VAR, STR) \
    char* VAR = (char*)rl_MemAlloc((strlen(STR) * sizeof(char*)) + 1); \
    strcpy(VAR, STR);

void SzRuntimeError(char* title, char* text);
void SzDebugInfo(char* text, bool* oneshot);
