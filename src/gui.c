#include "gui.h"

/// Creates a TextButton: a combination of rectangle, text and signals
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
    )
{
    SzEntity* t = CreateType("TextButton");

    SzTextButton* btn = (SzTextButton*)MemAlloc(sizeof(SzTextButton));
    
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
        btn->font         = GetFontDefault();
    else
        btn->font         = *font;
    if (fontShader == NULL)
        btn->fontShader   = SDFShader;
    else
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
    GuiLabelButton((Rectangle){btn->posX + btn->margin, btn->posY, btn->width, btn->height}, btn->text, NULL);
}

/// Draws a TextButton (obsolete)
void DrawTextButtonOld(SzTextButton* btn, bool sdf)
{
    Rectangle rec = {btn->posX, btn->posY, btn->width, btn->height};
    bool mouse_in = CheckCollisionPointRec(GetMousePosition(), rec);
    
    if (mouse_in && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        DrawRectangleRec(rec, btn->pressedColor);
    } else {
        DrawRectangleRec(rec, mouse_in ? btn->hoverColor : btn->idleColor);
    }
    
    if (sdf) {
        Shader shader = LoadShaderFromMemory(NULL, btn->fontShader);
        BeginShaderMode(shader);
    }
        DrawTextPro(btn->font, btn->text, (Vector2){rec.x + btn->textXOffset, rec.y + btn->textYOffset}, (Vector2){rec.x, rec.y}, 0, btn->fontSize, 0, btn->fontIdleColor);
    if (sdf) {
        EndShaderMode();
    }
}

void UnloadTextButton(SzTextButton* btn)
{
    UnloadFont(btn->font);
    MemFree(btn);
}

/* Methods:
    0 - Create font from TTF
    1 - Create font from image
    2 - Create font from raw code data
*/ 
SzEntity* CreateFont(unsigned char* fontSource, unsigned int sourceSize, int baseSize, int charsCount, FontCreationMethod method)
{
    SzEntity* t = CreateType("Font");

    Font* font = (Font*)MemAlloc(sizeof(Font));

    *font = (Font){0};
    font->baseSize   = baseSize;
    charsCount = (charsCount > 0) ? charsCount : 95;
    font->glyphCount = charsCount;
    unsigned char* font_data;
    switch (method)
    {
    case FONT_CREATION_METHOD_TTF:
        font->glyphPadding = 0; // FONT_TTF_DEFAULT_CHARS_PADDING
        font_data = LoadFileData((char*)fontSource, &sourceSize);
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
    font->glyphs = LoadFontData(font_data, sourceSize, font->baseSize, 0, font->glyphCount, FONT_DEFAULT);
    if (method != FONT_CREATION_METHOD_RAW)
        MemFree(font_data);
    Image atlas = GenImageFontAtlas(font->glyphs, &(font->recs), font->glyphCount, font->baseSize, font->glyphPadding, 0);
    font->texture = LoadTextureFromImage(atlas);
    
    SzEntity* image_data_t = NULL;
    // Update chars[i].image to use alpha, required to be used on ImageDrawText()
    for (int i = 0; i < font->glyphCount; i++)
    {
        UnloadImage(font->glyphs[i].image);
        font->glyphs[i].image = ImageFromImage(atlas, font->recs[i]);
        image_data_t = CreateType("Void");
        image_data_t->essence = font->glyphs[i].image.data;
        EnlistMemory(image_data_t, "free_at_shutdown");
    }
    
    UnloadImage(atlas);
    
    SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);
    
    t->essence = font;
    
    return t;
}

