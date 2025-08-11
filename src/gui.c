#include "gui.h"

/// Creates a TextButton: a combination of rectangle, text and signals
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
    )
{
    SzEntity* t = CreateType("TextButton");

    SzTextButton* btn = (SzTextButton*)rl_MemAlloc(sizeof(SzTextButton));
    
    *btn = (SzTextButton){0};
    btn->text             = text;
    btn->posX             = posX;
    btn->posY             = posY;
    btn->width            = width;
    btn->height           = height;
    btn->margin           = margin;
    btn->idleColor        = idleColor;
    btn->hoverColor       = hoverColor;
    btn->pressedColor     = pressedColor;
    if (font == NULL)
        btn->font         = rl_GetFontDefault();
    else
        btn->font         = *font;
    btn->fontShader   = fontShader;
    btn->fontSize         = fontSize;
    btn->fontIdleColor    = fontIdleColor;
    btn->fontHoverColor   = fontHoverColor;
    btn->fontPressedColor = fontPressedColor;
    btn->textXOffset      = textXOffset;
    btn->textYOffset      = textYOffset;

    t->essence = btn;
    
    return t;
}

/// Places elements contained by an SzConstruct in a line
void AdjustSequence(SzConstruct* construct, short alignType)
{
    
}

/// Draws a TextButton
void DrawTextButton(SzTextButton* btn)
{
    rl_GuiLabelButton((Rl_Rectangle){btn->posX + btn->margin, btn->posY, btn->width, btn->height}, btn->text);
}

/// Draws a TextButton (obsolete)
void DrawTextButtonOld(SzTextButton* btn, bool sdf)
{
    Rl_Rectangle rec = {btn->posX, btn->posY, btn->width, btn->height};
    bool mouse_in = rl_CheckCollisionPointRec(rl_GetMousePosition(), rec);
    
    if (mouse_in && rl_IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        rl_DrawRectangleRec(rec, btn->pressedColor);
    } else {
        rl_DrawRectangleRec(rec, mouse_in ? btn->hoverColor : btn->idleColor);
    }
    
    if (sdf) {
        Rl_Shader shader = rl_LoadShaderFromMemory(NULL, btn->fontShader);
        rl_BeginShaderMode(shader);
    }
        rl_DrawTextPro(btn->font, btn->text, (Rl_Vector2){rec.x + btn->textXOffset, rec.y + btn->textYOffset}, (Rl_Vector2){rec.x, rec.y}, 0, btn->fontSize, 0, btn->fontIdleColor);
    if (sdf) {
        rl_EndShaderMode();
    }
}

void UnloadTextButton(SzTextButton* btn)
{
    rl_UnloadFont(btn->font);
    rl_MemFree(btn);
}

/* Methods:
    0 - Create font from TTF
    1 - Create font from image
    2 - Create font from raw code data
*/ 
SzEntity* CreateFont(unsigned char* fontSource, int sourceSize, int baseSize, int charsCount, FontCreationMethod method)
{
    SzEntity* t = CreateType("Font");

    Rl_Font* font = (Rl_Font*)rl_MemAlloc(sizeof(Rl_Font));

    *font = (Rl_Font){0};
    font->baseSize   = baseSize;
    charsCount = (charsCount > 0) ? charsCount : 95;
    font->glyphCount = charsCount;
    unsigned char* font_data;
    switch (method)
    {
    case FONT_CREATION_METHOD_TTF:
        font->glyphPadding = 0; // FONT_TTF_DEFAULT_CHARS_PADDING
        font_data = rl_LoadFileData((char*)fontSource, &sourceSize);
        break;
    case FONT_CREATION_METHOD_IMAGE:
        
        break;
    case FONT_CREATION_METHOD_RAW:
        font_data = fontSource;
        font->glyphPadding = 4;
        break;
    default:
        break;
    }
    font->glyphs = rl_LoadFontData(font_data, sourceSize, font->baseSize, 0, font->glyphCount, FONT_DEFAULT);
    if (method != FONT_CREATION_METHOD_RAW)
        rl_MemFree(font_data);
    Rl_Image atlas = rl_GenImageFontAtlas(font->glyphs, &(font->recs), font->glyphCount, font->baseSize, font->glyphPadding, 0);
    font->texture = rl_LoadTextureFromImage(atlas);
    
    SzEntity* image_data_t = NULL;
    // Update chars[i].image to use alpha, required to be used on ImageDrawText()
    for (int i = 0; i < font->glyphCount; i++)
    {
        rl_UnloadImage(font->glyphs[i].image);
        font->glyphs[i].image = rl_ImageFromImage(atlas, font->recs[i]);
        image_data_t = CreateType("Void");
        image_data_t->essence = font->glyphs[i].image.data;
        EnlistMemory(image_data_t, "free_at_shutdown");
    }
    
    rl_UnloadImage(atlas);
    
    rl_SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);
    
    t->essence = font;
    
    return t;
}

