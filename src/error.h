#pragma once

#include <string.h>

#define CREATE_STR(VAR, STR) \
    char* VAR = (char*)MemAlloc((strlen(STR) * sizeof(char*)) + 1); \
    strcpy(VAR, STR);

void SzRuntimeError(char* title, char* text);
void SzDebugInfo(char* text);