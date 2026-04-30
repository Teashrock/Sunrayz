#pragma once

#include <raylib.h>
#include "error.h"

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
} SzConstruct;

SzConstruct* CreateConstruct(SzConstruct* parent);
