#pragma once

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define DYN_WIDTH  -1
#define DYN_HEIGHT -2

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
} TextButton;

TextButton* CreateButton(
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
    Color fontPressedColor
    );

void DrawButton(TextButton*, bool);
void FreeFreeables();
Font CreateFont(char* fontFile, int baseSize, int charsCount);
Font CreateSDFFont(char* fontFile, int baseSize, int charsCount);
void UpdateRec(Rectangle*);