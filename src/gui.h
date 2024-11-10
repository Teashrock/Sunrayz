#pragma once

#include <raylib.h>

#include "base.h"

typedef struct {
    float posX;
    float posY;
    float width;
    float height;
    float margin;
    RLRectangle shape;
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

SzEntity* CreateFont(unsigned char* fontSource,
    int sourceSize,
    int baseSize,
    int charsCount,
    FontCreationMethod method);
void DrawTextButton(SzTextButton* btn);
