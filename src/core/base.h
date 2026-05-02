#pragma once

#include <raylib.h>
#include "error.h"

#if _WIN32
static const char* pathDelimiter = "\\";
#else
static const char* pathDelimiter = "/";
#endif

typedef struct _SzEntity {
    struct _SzConstruct* parent;
    void* essence;
    char* type;
    struct _SzEntity* next;
    int* id;
} SzEntity;

typedef struct _SzConstruct {
    struct _SzConstruct* parent;
    SzEntity* parts;
    struct _SzConstruct* child;
    struct _SzConstruct** children;
} SzConstruct;

SzConstruct* CreateConstruct(SzConstruct* parent);
