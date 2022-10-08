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
    size_t name_len;
    SzTag* entities;
    char* types;
    size_t types_len;
    struct _Group* next;
} Group;

Group* CreateGroup(char* name, char* types);
void DestroyGroup(char* name);
void DestroyGroupByRef(Group*);
int* EnlistMemory(SzType* obj, char* group);
int* EnlistMemoryByRef(SzType* obj, Group* group);
int ExcludeMemory(int id, char* group);
int ExcludeMemoryByRef(int id, Group* group);