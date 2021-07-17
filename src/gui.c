#include "gui.h"
#include "error.h"

// Pool for freeable objects
static void** freeables    = NULL;
static int freeablesNumber = 0;

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
    )
{
    TextButton btn = {0};
    btn.text             = text;
    btn.posX             = posX;
    btn.posY             = posY;
    btn.width            = width;
    btn.height           = height;
    btn.idleColor        = idleColor;
    btn.hoverColor       = hoverColor;
    btn.pressedColor     = pressedColor;
    if (font == NULL)
        btn.font         = GetFontDefault();
    else
        btn.font         = *font;
    if (fontShader == NULL)
        btn.fontShader   = SDFShader;
    else
        btn.fontShader   = fontShader;
    btn.fontSize         = fontSize;
    btn.fontIdleColor    = fontIdleColor;
    btn.fontHoverColor   = fontHoverColor;
    btn.fontPressedColor = fontPressedColor;
    
    TextButton* ptr = (TextButton*)malloc(sizeof(float)*4 + sizeof(Color)*6 + sizeof(Font) + sizeof(int) + sizeof(char*));
    AllocError(ptr, "Failed to allocate pointer!");
    
    *ptr = btn;
    freeables = malloc(sizeof(TextButton*));
    AllocError(freeables, "Failed to allocate pointer!");
    
    *freeables = ptr;
    freeablesNumber++;
    return ptr;
}

void DrawButton(TextButton* btn, bool sdf)
{
    Rectangle rec = {btn->posX, btn->posY, btn->width, btn->height};
    bool mouse_in = CheckCollisionPointRec(GetMousePosition(), rec);
    
    if (mouse_in && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        DrawRectangleRec(rec, btn->pressedColor);
    } else {
        DrawRectangleRec(rec, mouse_in ? btn->hoverColor : btn->idleColor);
    }
    
    rec.x += 5;
    rec.y += 7;
    Shader shader = LoadShaderFromMemory(NULL, btn->fontShader);
    if (sdf) BeginShaderMode(shader);
        DrawTextRec(btn->font, btn->text, rec, btn->fontSize, 0, false, btn->fontIdleColor);
    if (sdf) EndShaderMode();
}

void FreeFreeables()
{
    for (; freeablesNumber > 0; freeablesNumber--)
    {
        free(*freeables);
    }
}

Font CreateFont(char* fontFile, int baseSize, int charsCount)
{
    Font font = {0};
    font.baseSize   = baseSize;
    font.charsCount = charsCount;
    unsigned int fileSize = 0;
    font.chars = LoadFontData(LoadFileData(fontFile, &fileSize), fileSize, font.baseSize, 0, 0, FONT_DEFAULT);
    font.texture = LoadTextureFromImage(GenImageFontAtlas(font.chars, &font.recs, font.charsCount, font.baseSize, 0, 1));
    return font;
}

Font CreateSDFFont(char* fontFile, int baseSize, int charsCount)
{
    Font font = {0};
    font.baseSize   = baseSize;
    font.charsCount = charsCount;
    unsigned int fileSize = 0;
    font.chars = LoadFontData(LoadFileData(fontFile, &fileSize), fileSize, font.baseSize, 0, 0, FONT_SDF);
    font.texture = LoadTextureFromImage(GenImageFontAtlas(font.chars, &font.recs, font.charsCount, font.baseSize, 0, 1));
    SetTextureFilter(font.texture, FILTER_BILINEAR);
    return font;
}

// Updates a rectangle containing dynamic values
void UpdateRec(Rectangle* rec)
{
    if (rec->width == DYN_WIDTH)
        rec->width = GetScreenWidth();
    if (rec->height == DYN_HEIGHT)
        rec->height = GetScreenHeight();
}