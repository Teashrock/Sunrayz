#pragma once

#include "typing.h"
#include <stddef.h>

typedef struct _SzTag
{
    SzType* entity;
    struct _SzTag* next;
} SzTag;


typedef struct _Group
{
    char* name;
    SzTag* entities;
    char* types;
    struct _Group* next;
} Group;

static Group* gGroupEnv = NULL;
static Group* gCurrentGroup = NULL;

void CreateGroup(char* name, char* types);
void DestroyGroup(char* name);
int EnlistMemory(SzType* obj, char* group);
int ExcludeMemory(int id, char* group);