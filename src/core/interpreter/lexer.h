// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#pragma once

#include <stdio.h>


typedef struct _SzToken {
    char* token;
    struct _SzToken* next;
} SzToken;

/// Called first in the process of parsing:
/// picks up the tokens from the code,
/// separating them from syntactic water;
/// takes opened file pointer as an argument
SzToken* Tokenise(const char* fileName);
