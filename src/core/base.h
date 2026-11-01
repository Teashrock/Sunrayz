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

/// Is returned by StringReadUntilArr
/// terminator is the exact symbol string ended with
/// position is the file position on the terminator of the string
struct StringReadResult{
    char terminator;
    long position;
};

/// Reads a string from an opened file
/// up to the provided character (char until),
/// then writes it by a provided char* pointer (dest).
/// Returns file position on the terminator of the string.
/// No need to allocate dest in advance.
long StringReadUntil(FILE* source, char until, char** dest);

/// Reads a string from an opened file
/// up to a character in of the provided strings (const char* const until[]),
/// then writes it by a provided char* pointer (dest).
/// Returns struct StringReadResult.
/// No need to allocate dest in advance.
struct StringReadResult StringReadUntilArr(FILE* source, const char* const until[], size_t arrAmount, char** dest);

/// Checks if a string can be seamlessly converted into an integer.
/// Warning: expects a zstring.
bool StringIsInteger(const char* const string);

/// Checks if a string can be seamlessly converted into a float or double.
/// Warning: expects a zstring.
bool StringIsFraction(const char* const string);

/// Checks if a string can be seamlessly converted into a boolean.
/// Warning: expects a zstring.
bool StringIsBool(const char* const string);
