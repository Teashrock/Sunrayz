#pragma once

//#include <raylib.h>
#include <raygui.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float posX;
    float posY;
    float width;
    float height;
    float margin;
    Rectangle shape;
    Color idleColor;
    Color hoverColor;
    Color pressedColor;
    Font font;
    char* fontShader;
    int fontSize;
    Color fontIdleColor;
    Color fontHoverColor;
    Color fontPressedColor;
    char* text;
    float textXOffset;
    float textYOffset;
} SzTextButton;

typedef struct _SzType
{
    struct _SzConstruct* parent;
    void* entity;
    char* type;
    struct _SzType* next;
    int* id;
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

typedef struct _SzTag
{
    SzEntity* entity;
    struct _SzTag* next;
} SzTag;

typedef struct _Group
{
    char* name;
    size_t* name_len;
    SzTag* entities;
    char* types;
    size_t* types_len;
    struct _Group* next;
} Group;

SzEntity* CreateTextButton(
    char* text,
    float posX,
    float posY,
    float width,
    float height,
    float margin,
    Color idleColor,
    Color hoverColor,
    Color pressedColor,
    Font* font,
    char* fontShader,
    int fontSize,
    Color fontIdleColor,
    Color fontHoverColor,
    Color fontPressedColor,
    float textXOffset,
    float textYOffset
);

void DrawTextButton(SzTextButton* btn);
SzEntity* CreateFont(unsigned char* fontSource,
    unsigned int sourceSize,
    int baseSize,
    int charsCount,
    FontCreationMethod method);
SzEntity* CreateRec(float x, float y, float width, float height);
SzConstruct* CreateConstruct(SzConstruct* parent);
SzEntity* CreateType(char* typeName);
Group* CreateGroup(char* name, char* types);
void DestroyGroup(char* name);
void DestroyGroupByRef(Group*);
int* EnlistMemory(SzEntity* obj, char* group);
int* EnlistMemoryByRef(SzEntity* obj, Group* group);
int ExcludeMemory(int id, char* group);
int ExcludeMemoryByRef(int id, Group* group);
void AddEntity(SzConstruct* cnst, SzEntity* ent);