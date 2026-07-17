#pragma once

#include <raylib.h>
#include <stdio.h>

#if _WIN32
static const char* pathDelimiter = "\\";
#else
static const char* pathDelimiter = "/";
#endif

long StringReadUntil(FILE* f, char until, char** dest);
bool StringIsInteger(char* string);
bool StringIsBool(char* string);
