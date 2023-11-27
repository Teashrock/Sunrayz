#pragma once

#include <raygui.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

typedef struct _SzSignal {
    char* name;
    SzError (*trigger)(void);
    SzError (*behaviour)(void);
    struct _SzSignal* next;
} SzSignal;

typedef struct _SzEntity {
    struct _SzConstruct* parent;
    void* essence;
    char* type;
    struct _SzEntity* next;
    int* id;
    SzSignal* signals;
} SzEntity;

typedef struct _SzConstruct {
    struct _SzConstruct* parent;
    unsigned int zorder;
    SzEntity* parts;
    struct _SzConstruct* child;
} SzConstruct;

typedef enum {
    FONT_CREATION_METHOD_TTF,
    FONT_CREATION_METHOD_IMAGE,
    FONT_CREATION_METHOD_RAW
} FontCreationMethod;

typedef struct _SzTag {
    SzEntity* essence;
    struct _SzTag* next;
} SzTag;

typedef struct _Group {
    char* name;
    size_t* name_len;
    SzTag* entities;
    char* types;
    size_t* types_len;
    struct _Group* next;
} Group;

void AddEntity(SzConstruct* cnst, SzEntity* ent);
SzConstruct* CreateConstruct(SzConstruct* parent);
Group* CreateGroup(char* name, char* types);
SzEntity* CreateRec(float x, float y, float width, float height);
SzSignal* CreateSignal(char* name, SzError (*trigger)(void), SzError (*behaviour)(void));
SzEntity* CreateType(char* typeName);
void DestroyGroup(char* name);
void DestroyGroupByRef(Group*);
int* EnlistMemory(SzEntity* obj, char* group);
int* EnlistMemoryByRef(SzEntity* obj, Group* group);
SzError ExcludeMemory(int id, char* group);
int ExcludeMemoryByRef(int id, Group* group);
SzError RemoveEntity(SzConstruct* cnst, int* id);
