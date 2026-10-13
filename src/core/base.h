// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#pragma once

#include <raylib.h>
#include <stdio.h>

#if _WIN32
static const char* pathDelimiter = "\\";
#else
static const char* pathDelimiter = "/";
#endif

long StringReadUntil(FILE* source, char until, char** dest);
long StringReadUntilArr(FILE* source, const char* const until, char** dest);
bool StringIsInteger(const char* const string);
bool StringIsFraction(const char* const string);
bool StringIsBool(const char* const string);
