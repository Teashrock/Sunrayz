#include "base.h"
#include "error.h"

#ifdef __HAIKU__
static char* SDFShader = "#version 140\n\n\
varying vec2 fragTexCoord;\
varying vec4 fragColor;\
uniform sampler2D texture0;\
uniform vec4 colDiffuse;\
const float smoothing = 1.0/16.0;\n\n\
void main()\
{\
    // Texel color fetching from texture sampler\
    // NOTE: Calculate alpha using signed distance field (SDF)\
    float distance = texture2D(texture0, fragTexCoord).a;\n\n\
    if (distance > 0.0)\
        float alpha = 1.0;\
    else\
        float alpha = 0.0;\
    gl_FragColor = vec4(fragColor.rgb, alpha);\
}\
\0";
#else
static char* SDFShader = "#version 330\n\n\
in vec2 fragTexCoord;\
in vec4 fragColor;\
uniform sampler2D texture0;\
uniform vec4 colDiffuse;\
out vec4 finalColor;\n\n\
void main()\
{\
    // Texel color fetching from texture sampler\
    // NOTE: Calculate alpha using signed distance field (SDF)\
    float distanceFromOutline = texture(texture0, fragTexCoord).a - 0.5;\
    float distanceChangePerFragment = length(vec2(dFdx(distanceFromOutline), dFdy(distanceFromOutline)));\
    float alpha = smoothstep(-distanceChangePerFragment, distanceChangePerFragment, distanceFromOutline);\n\n\
    finalColor = vec4(fragColor.rgb, fragColor.a*alpha);\
}\
\0";
#endif

SzEntity* CreateRec(float x, float y, float width, float height)
{
    SzEntity* t = CreateType("RLRectangle");

    RLRectangle* rect = (RLRectangle*)MemAlloc(sizeof(RLRectangle));
    
    *rect = (RLRectangle){
        .x = x,
        .y = y,
        .width = width,
        .height = height
    };

    t->essence = rect;
    
    return t;
}

SzConstruct* CreateConstruct(SzConstruct* parent)
{
    SzConstruct* c = (SzConstruct*)MemAlloc(sizeof(SzConstruct));

    *c = (SzConstruct){
        .parent = parent,
        .child = NULL,
        .parts = NULL
    };
    if (parent == NULL)
        c->zorder = 0;
    else {
        c->zorder = parent->zorder++;
        parent->child = c;
    }

    return c;
}

/// Creates an instance of Sunrayz visible type.
SzEntity* CreateType(char* typeName) {
    SzEntity* t = (SzEntity*)MemAlloc(sizeof(SzEntity));
    *t = (SzEntity){
        .essence = NULL,
        .type = (char*)MemAlloc((sizeof(char) * strlen(typeName)) + 1),
        .id = (int*)MemAlloc(sizeof(int)),
    };
    strcpy(t->type, typeName);
    *(t->id) = rand();
    return t;
}

SzSignal* CreateSignal(char* name, SzError (*trigger)(void), SzError (*behaviour)(void)) {
    SzSignal* s = (SzSignal*)MemAlloc(sizeof(SzSignal));
    *s = (SzSignal){
        .name = name,
        .trigger = NULL,
        .behaviour = NULL
    };
    return s;
}

void AddSignal(SzEntity* e, SzSignal* sig) {
    SzSignal* temp = e->signals;
    while (temp != NULL) {
        temp = temp->next;
    }
    temp = sig;
}

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

void SzCheckGroupType(SzEntity* obj, char* group)
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
int* EnlistMemory(SzEntity* obj, char* group)
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
        .essence = obj,
        .next = NULL
    };
    
    if (backTag == NULL)
        gCurrentGroup->entities = tag;
    else
        backTag->next = tag;

    return tag->essence->id;
}

/// Adds an object into a group
int* EnlistMemoryByRef(SzEntity* obj, Group* group)
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
        .essence = obj,
        .next = NULL
    };

    if (backTag == NULL)
        group->entities = tag;
    else
        backTag->next = tag;

    return tag->essence->id;
}

/// Excludes an object from a memory group
SzError ExcludeMemory(int id, char* group)
{
    FIND_GROUP(group)
    
    SzTag* prevEl = NULL;
    SzTag* currEl = NULL;
    currEl = gCurrentGroup->entities;
    loop:
    if (currEl == NULL)
        return GENERIC;
    else if (*(currEl->essence->id) == id) {
        if (prevEl == NULL)
            gCurrentGroup->entities = currEl->next;
        else
            prevEl->next = currEl->next;
    } else {
        prevEl = currEl;
        currEl = currEl->next;
        goto loop;
    }

    return OK;
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
    else if (*(currEl->essence->id) == id) {
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

/// Adds an entity to an SzConstruct
void AddEntity(SzConstruct* cnst, SzEntity* ent) {
    SzEntity* temp = NULL;
    if (cnst->parts == NULL) {
        cnst->parts = ent;
    } else {
        temp = cnst->parts;
        loop:
        if (temp->next != NULL) {
            temp = temp->next;
            goto loop;
        } else {
            temp->next = ent;
        }
    }
}

SzError RemoveEntity(SzConstruct* cnst, int* id) {
    SzEntity* prev_entity = NULL;
    SzEntity* entity = cnst->parts;
    while (entity->essence != NULL) {
        if (*(entity->id) == *id) {
            
            return OK;
        } else {
            prev_entity = entity;
            entity = entity->next;
        }
    }

    return NOTFOUND;
}

void ArrangeSequence(SzConstruct* cnst) {
    
}
