#pragma once

#include <raylib.h>

#include "runtime.h"

typedef enum {
    ENTITY_TYPE_TEXTURE
} SzEntityType;

typedef struct _SzEntity {
    struct _SzConstruct* parent;
    void* essence;
    SzEntityType type;
    SzVariable* params;
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
SzEntity* CreateVisualEntity(SzConstruct* parent, Texture2D texture, Rectangle sizeRect);
void AttachEntity(SzConstruct* construct, SzEntity* entity);
