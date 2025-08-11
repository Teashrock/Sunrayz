#pragma once

#include <raylib.h>

#include "base.h"

typedef struct {
    float posX;
    float posY;
    float width;
    float height;
    float margin;
    Rl_Rectangle shape;
    Rl_Color idleColor;
    Rl_Color hoverColor;
    Rl_Color pressedColor;
    Rl_Font font;
    char* fontShader;
    int fontSize;
    Rl_Color fontIdleColor;
    Rl_Color fontHoverColor;
    Rl_Color fontPressedColor;
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
    Rl_Color idleColor,
    Rl_Color hoverColor,
    Rl_Color pressedColor,
    Rl_Font* font,
    char* fontShader,
    int fontSize,
    Rl_Color fontIdleColor,
    Rl_Color fontHoverColor,
    Rl_Color fontPressedColor,
    float textXOffset,
    float textYOffset
);

SzEntity* CreateFont(unsigned char* fontSource,
    int sourceSize,
    int baseSize,
    int charsCount,
    FontCreationMethod method);
void DrawTextButton(SzTextButton* btn);
