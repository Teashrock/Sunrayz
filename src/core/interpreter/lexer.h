// SPDX-License-Identifier: BSD 3-Clause
// Copyright (c) 2021-2026 Teashrock

#pragma once

#include <stdio.h>


typedef struct _SzToken {
    char* token;
    struct _SzToken* next;
} SzToken;

SzToken* Tokenise(const char* fileName);
