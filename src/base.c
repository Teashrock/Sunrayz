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

    t->entity = btn;
    
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
        image_data_t->entity = font->glyphs[i].image.data;
        EnlistMemory(image_data_t, "free_at_shutdown");
    }
    
    UnloadImage(atlas);
    
    SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);
    
    t->entity = font;
    
    return t;
}

SzEntity* CreateRec(float x, float y, float width, float height)
{
    SzEntity* t = CreateType("Rectangle");

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
SzEntity* CreateType(char* typeName)
{
    SzEntity* t = (SzEntity*)MemAlloc(sizeof(SzEntity));
    *t = (SzEntity){
        .entity = NULL,
        .type = (char*)MemAlloc((sizeof(char) * strlen(typeName)) + 1),
        .id = (int*)MemAlloc(sizeof(int)),
    };
    strcpy(t->type, typeName);
    *(t->id) = rand();
    return t;
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
        .entity = obj,
        .next = NULL
    };
    
    if (backTag == NULL)
        gCurrentGroup->entities = tag;
    else
        backTag->next = tag;

    return tag->entity->id;
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
        .entity = obj,
        .next = NULL
    };

    if (backTag == NULL)
        group->entities = tag;
    else
        backTag->next = tag;

    return tag->entity->id;
}

/// Excludes an object from a memory group
int ExcludeMemory(int id, char* group)
{
    FIND_GROUP(group)
    
    SzTag* prevEl = NULL;
    SzTag* currEl = NULL;
    currEl = gCurrentGroup->entities;
    loop:
    if (currEl == NULL)
        return 1;
    else if (*(currEl->entity->id) == id) {
        if (prevEl == NULL)
            gCurrentGroup->entities = currEl->next;
        else
            prevEl->next = currEl->next;
    } else {
        prevEl = currEl;
        currEl = currEl->next;
        goto loop;
    }

    return 0;
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
    else if (*(currEl->entity->id) == id) {
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

void ArrangeSequence(SzConstruct* cnst) {
    
}