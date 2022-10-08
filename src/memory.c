#include "memory.h"

#include <raylib.h>
#include <string.h>
#include <stdio.h>

#include "error.h"
#include "stdbool.h"

static Group* gGroupEnv = NULL;
static Group* gCurrentGroup = NULL;

#define FIND_GROUP(VAR)                                         \
    gCurrentGroup = gGroupEnv;                                  \
    while (true) {                                              \
        if (gCurrentGroup == NULL)                              \
            SzRuntimeError(NULL, "No such group exists!");      \
        else if (strcmp(gCurrentGroup->name, VAR) != 0)         \
            gCurrentGroup = gCurrentGroup->next;                \
        else break;                                             \
    }

/// Creates a group of objects in memory
Group* CreateGroup(char* name, char* types)
{
    if (gGroupEnv == NULL) {
        Group* group = MemAlloc(sizeof(Group));
        group->entities = NULL;
        group->name_len = (size_t*)MemAlloc(sizeof(size_t));
        *(group->name_len) = strlen(name);
        group->name = (char*)MemAlloc((sizeof(char) * *(group->name_len)) + 1);
        strcpy(group->name, name);
        group->types_len = (size_t*)MemAlloc(sizeof(size_t));
        *(group->types_len) = strlen(types);
        group->types = (char*)MemAlloc((sizeof(char) * *(group->types_len)) + 1);
        strcpy(group->types, types);
        gGroupEnv = group;
        return group;
    } else {
        gCurrentGroup = gGroupEnv;
        loop: {
            if (strcmp(gCurrentGroup->name, name) == 0)
                SzRuntimeError("Failed to create group!", "Group with this name already exists.");
            if (gCurrentGroup->next != NULL) {
                gCurrentGroup = gCurrentGroup->next;
                goto loop;
            } else {
                Group* group = MemAlloc(sizeof(Group));
                group->entities = NULL;
                group->name_len = (size_t*)MemAlloc(sizeof(size_t));
                *(group->name_len) = strlen(name);
                group->name = (char*)MemAlloc((sizeof(char) * *(group->name_len)) + 1);
                strcpy(group->name, name);
                group->types_len = (size_t*)MemAlloc(sizeof(size_t));
                *(group->types_len) = strlen(types);
                group->types = (char*)MemAlloc((sizeof(char) * *(group->types_len)) + 1);
                strcpy(group->types, types);
                gCurrentGroup->next = group;
                return group;
            }
        }
    }
}

/// Destroys a group, freeing any memory it was using
void DestroyGroup(char* name)
{
    FIND_GROUP(name)
    
    SzTag* backLink;
    SzTag* tag = gCurrentGroup->entities;
    // Deleting all tags of the group before deleting the group itself
    loop:
    if (tag == NULL) {
        gCurrentGroup->entities = NULL;
        gCurrentGroup->next = NULL;
        MemFree(gCurrentGroup->types);
        MemFree(gCurrentGroup->types_len);
        MemFree(gCurrentGroup->name);
        MemFree(gCurrentGroup->name_len);
        MemFree(gCurrentGroup);
    } else {
        backLink = tag->next;
        MemFree(tag);
        tag = backLink;
        goto loop;
    }
}

void DestroyGroupByRef(Group* group)
{
    SzTag* backLink;
    SzTag* tag = group->entities;
    // Deleting all tags of the group before deleting the group itself
    loop:
    if (tag == NULL)
        MemFree(group);
    else {
        backLink = tag->next;
        MemFree(tag);
        tag = backLink;
        goto loop;
    }
}

void SzCheckGroupType(SzType* obj, char* group)
{
    // Type checking
    CREATE_STR(typesStr, gCurrentGroup->types)
    char* type = strtok(typesStr, ",");

    iterTypes:
    if (type == NULL) { // If neither token equals the needed type, throw a runtime error
        char* errorstr = (char*)MemAlloc(sizeof(char*));
        sprintf(errorstr, "Wrong type has been tried to add into group \"%s\".", group);
        SzRuntimeError(NULL, errorstr);
    }
    if (strcmp(type, obj->type) != 0) { // If the current token in 'type' doesn't respect 'obj->type',
        type = strtok(NULL, ",");       // switch to next token
        goto iterTypes;                 // and return to check,
    }                                   // else pass further
    MemFree(typesStr);
    typesStr = NULL;
}

/// Adds an object into a group
int* EnlistMemory(SzType* obj, char* group)
{
    FIND_GROUP(group)
    SzCheckGroupType(obj, group);

    SzTag* backTag = NULL;
    SzTag* tag = gCurrentGroup->entities;
    iterEntity:
    if (tag != NULL) {
        backTag = tag;
        tag = tag->next;
        goto iterEntity;
    }

    // Tag creation
    tag = (SzTag*)MemAlloc(sizeof(SzTag));
    *tag = (SzTag){
        .entity = obj,
        .next = NULL
    };
    
    if (backTag == NULL)
        gCurrentGroup->entities = tag;
    else
        backTag->next = tag;

    return tag->entity->id;
}

/// Adds an object into a group
int* EnlistMemoryByRef(SzType* obj, Group* group)
{
    // Type checking
    char* type = strtok(gCurrentGroup->types, ",");

    iterTypes:
    if (type == NULL) { // If neither token equals the needed type, throw a runtime error
        char* errorstr = (char*)MemAlloc(sizeof(char*));
        sprintf(errorstr, "Wrong type has been tried to add into group \"%s\".", group->name);
        SzRuntimeError(NULL, errorstr);
    }
    if (strcmp(type, obj->type) != 0) { // If the current token in 'type' doesn't respect 'obj->type',
        type = strtok(NULL, ",");       // switch to next token
        goto iterTypes;                 // and return to check,
    }                                   // else pass further

    SzTag* backTag = NULL;
    SzTag* tag = group->entities;
    iterEntity:
    if (tag != NULL) {
        backTag = tag;
        tag = tag->next;
        goto iterEntity;
    }

    // Tag creation
    tag = (SzTag*)MemAlloc(sizeof(SzTag));
    *tag = (SzTag){
        .entity = obj,
        .next = NULL
    };

    if (backTag == NULL)
        group->entities = tag;
    else
        backTag->next = tag;

    return tag->entity->id;
}

/// Excludes an object from a memory group
int ExcludeMemory(int id, char* group)
{
    FIND_GROUP(group)
    
    SzTag* prevEl = NULL;
    SzTag* currEl = NULL;
    currEl = gCurrentGroup->entities;
    loop:
    if (currEl == NULL)
        return 1;
    else if (*(currEl->entity->id) == id) {
        if (prevEl == NULL)
            gCurrentGroup->entities = currEl->next;
        else
            prevEl->next = currEl->next;
    } else {
        prevEl = currEl;
        currEl = currEl->next;
        goto loop;
    }

    return 0;
}

/// Excludes an object from a memory group
int ExcludeMemoryByRef(int id, Group* group)
{
    SzTag* prevEl = NULL;
    SzTag* currEl = NULL;
    currEl = group->entities;
    loop:
    if (currEl == NULL)
        return 1;
    else if (*(currEl->entity->id) == id) {
        if (prevEl == NULL)
            group->entities = currEl->next;
        else
            prevEl->next = currEl->next;
    } else {
        prevEl = currEl;
        currEl = currEl->next;
        goto loop;
    }

    return 0;
}