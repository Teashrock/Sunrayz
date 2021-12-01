#include "memory.h"

#include <raylib.h>
#include <string.h>

#include "error.h"
#include "stdbool.h"

/// Creates a group of objects in memory
void CreateGroup(char* name, char* types)
{
    if (gGroupEnv == NULL) {
        Group* group = MemAlloc(sizeof(Group));
        group->entities = NULL;
        group->name = name;
        group->types = types;
        gGroupEnv = group;
        return;
    } else {
        gCurrentGroup = gGroupEnv;
        loop: {
            if (gCurrentGroup->name == name)
                SzError("Failed to create group!", "Group with this name already exists.");
            if (gCurrentGroup->next != NULL) {
                gCurrentGroup = gCurrentGroup->next;
                goto loop;
            } else {
                Group* group = MemAlloc(sizeof(Group));
                group->entities = NULL;
                group->name = name;
                group->types = types;
                gCurrentGroup->next = group;
            }
        }
    }
}

/// Destroys a group, freeing a memory it was using
void DestroyGroup(char* name)
{
    gCurrentGroup = gGroupEnv;
    while (gCurrentGroup->name != name)
        gCurrentGroup = gCurrentGroup->next;
    SzTag* backLink;
    SzTag* tag = gCurrentGroup->entities;
    // Deleting all tags of the group before deleting the group itself
    loop:
    if (tag == NULL)
        MemFree(gCurrentGroup);
    else {
        backLink = tag->next;
        MemFree(tag);
        tag = backLink;
        goto loop;
    }
}

/// Adds an object into a group
int EnlistMemory(SzType* obj, char* group)
{
    gCurrentGroup = gGroupEnv; // Selecting the beginning as the current group
    while (gCurrentGroup->name != group)
    {
        gCurrentGroup = gCurrentGroup->next;
    }
    SzTag* tag = gCurrentGroup->entities;
    bool isFirst = true;
    loopIterElements:
    if (tag == NULL) {
        char* ts = gCurrentGroup->types;
        char* type;
        loopIterTypes:
        type = strtok(ts, ';');
        if (type == NULL)
            SzError(NULL, "Wrong type was tried to be added in a group.");
        if (type != obj->type) {
            ts = NULL;
            goto loopIterTypes;
        }
        tag = (SzTag*)MemAlloc(sizeof(SzTag));
        tag->entity = obj;
        tag->next = NULL;
        if (isFirst)
            gGroupEnv->entities = tag;
        else
            gCurrentGroup->entities = tag;
    } else {
        if (isFirst)
            isFirst = false;
        tag = tag->next;
        goto loopIterElements;
    }

    return tag->entity->id;
}

/// Excludes an object from a memory group
int ExcludeMemory(int id, char* group)
{
    gCurrentGroup = gGroupEnv;
    while (gCurrentGroup->name != group)
    {
        gCurrentGroup = gCurrentGroup->next;
        if (gCurrentGroup == NULL)
            return 2;
    }
    SzTag* prevEl = NULL;
    SzTag* currEl = NULL;
    currEl = gCurrentGroup->entities;
    loop:
    if (currEl == NULL)
        return 1;
    else if (currEl->entity->id == id) {
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