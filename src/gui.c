#include "gui.h"
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

/// Creates a TextButton: a combination of rectangle, text and signals
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
    )
{
    SzType* t = CreateType("TextButton");

    TextButton* btn = (TextButton*)MemAlloc(sizeof(TextButton));
    
    *btn = (TextButton){0};
    btn->text             = text;
    btn->posX             = posX;
    btn->posY             = posY;
    btn->width            = width;
    btn->height           = height;
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

    t->entity = btn;
    
    return t;
}

/// Draws a TextButton
void DrawTextButton(TextButton* btn, bool sdf)
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

void UnloadTextButton(TextButton* btn)
{
    UnloadFont(btn->font);
    MemFree(btn);
}

/* Methods:
    0 - Create font from TTF
    1 - Create font from image
    2 - Create font from raw code data
*/ 
SzType* CreateFont(unsigned char* fontSource, unsigned int sourceSize, int baseSize, int charsCount, FontCreationMethod method)
{
    SzType* t = CreateType("Font");

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
        free(font_data);
    Image atlas = GenImageFontAtlas(font->glyphs, &(font->recs), font->glyphCount, font->baseSize, font->glyphPadding, 0);
    font->texture = LoadTextureFromImage(atlas);
    
    // Update chars[i].image to use alpha, required to be used on ImageDrawText()
    for (int i = 0; i < font->glyphCount; i++)
    {
        UnloadImage(font->glyphs[i].image);
        font->glyphs[i].image = ImageFromImage(atlas, font->recs[i]);
        SzType* image_t = CreateType("Void");
        image_t->entity = font->glyphs[i].image.data;
        EnlistMemory(image_t, "free_at_shutdown");
    }
    
    UnloadImage(atlas);

    SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);

    t->entity = font;
    
    return t;
}

SzType* CreateRec(float x, float y, float width, float height)
{
    SzType* t = CreateType("Rectangle");

    Rectangle* rect = (Rectangle*)MemAlloc(sizeof(Rectangle));
    
    *rect = (Rectangle){
        .x = x,
        .y = y,
        .width = width,
        .height = height
    };

    t->entity = rect;
    
    return t;
}