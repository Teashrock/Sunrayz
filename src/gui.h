#pragma once

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

typedef struct
{
    float posX;
    float posY;
    float width;
    float height;
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
} TextButton;

typedef enum
{
    FONT_CREATION_METHOD_TTF,
    FONT_CREATION_METHOD_IMAGE,
    FONT_CREATION_METHOD_RAW
} FontCreationMethod;

SzType* CreateTextButton(
    char* text,
    float posX,
    float posY,
    float width,
    float height,
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

void DrawTextButton(TextButton* btn, bool sdf);
SzType* CreateFont(unsigned char* fontSource, unsigned int sourceSize, int baseSize, int charsCount, FontCreationMethod method);
SzType* CreateRec(float x, float y, float width, float height);