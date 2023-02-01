//#include <raygui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "base.h"

#include "assets_gen/fonts/fonts.h"

const int uniValues[] = {
    30 // Menu and panel collision point
};

#ifdef __HAIKU__
static char* APP_LOC = NULL;

char *strsep(char **stringp, const char *delim) {
    char *rv = *stringp;
    if (rv) {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0; }
    return rv;
}
#endif

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------

    #define screenWidth  800
    #define screenHeight 600
    
    #ifdef __HAIKU__
    //image_info info;
    //int cookie = 0;
    //get_next_image_info(0, &cookie, &info);
    char* xstr = argv[0];
    char* buf = NULL;
    char* tk = "";
    while((buf = strsep(&xstr, "/"))) {
    	strcat(APP_LOC, tk);
    	strcat(APP_LOC, "/");
    	tk = buf;
    }
    //printf("%s\n", APP_LOC);
    exit(0);
    #endif

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Sunrayz");
    SetWindowMinSize(screenWidth, screenHeight);
    SetWindowSize(screenWidth, screenHeight);
    MaximizeWindow();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    srand(time(NULL));
    CreateGroup("free_at_shutdown", "Rectangle,TextButton,Font,Void");

    SzConstruct* menuPanel = CreateConstruct(NULL);
    SzConstruct* panel1    = CreateConstruct(menuPanel);

    SzEntity* panel = CreateRec(0, 0, GetScreenWidth(), uniValues[0]);
    AddEntity(menuPanel, panel);
    SzEntity* sceneMenu  = CreateRec(0, uniValues[0], 260, GetScreenHeight());

    SzEntity* buttonDefaultFont = CreateFont(
        GetFontLibertinusSerif_Regular(),
        GetFontLibertinusSerif_RegularSize(),
        24,
        2048,
        FONT_CREATION_METHOD_RAW
    );
	GuiSetFont(*((Font*)buttonDefaultFont->entity));
            
    #define TEST_TEXT_MARGIN 5
    
    SzEntity* fileMenuButton = CreateTextButton(
        "File",
        0, 0,
        45, 30,
        TEST_TEXT_MARGIN,
        WHITE,
        LIGHTGRAY,
        GREEN,
        NULL, // TODO: Set the default font to render automatically, else override
        NULL,
        30,
        BLACK,
        BLACK,
        BLACK,
        5,
        2.1
    );

    SzEntity* editMenuButton = CreateTextButton(
        "Edit",
        0 + ((SzTextButton*)fileMenuButton->entity)->posX + ((SzTextButton*)fileMenuButton->entity)->width, 0,
        45, 30,
        TEST_TEXT_MARGIN,
        WHITE,
        LIGHTGRAY,
        GREEN,
        NULL, // TODO: Set the default font to render automatically, else override
        NULL,
        30,
        BLACK,
        BLACK,
        BLACK,
        5,
        2.1
    );

    SzEntity* aboutMenuButton = CreateTextButton(
        "About",
        0 + ((SzTextButton*)editMenuButton->entity)->posX + ((SzTextButton*)editMenuButton->entity)->width, 0,
        45, 30,
        TEST_TEXT_MARGIN,
        WHITE,
        LIGHTGRAY,
        GREEN,
        NULL, // TODO: Set the default font to render automatically, else override
        NULL,
        30,
        BLACK,
        BLACK,
        BLACK,
        5,
        2.1
    );

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GRAY);

            DrawRectangleRec(*(Rectangle*)panel->entity, WHITE);

            DrawTextButton((SzTextButton*)fileMenuButton->entity);
            //GuiLabelButton((Rectangle){((SzTextButton*)editMenuButton->entity)->posX + ((SzTextButton*)editMenuButton->entity)->margin, ((SzTextButton*)editMenuButton->entity)->posY, ((SzTextButton*)editMenuButton->entity)->width, ((SzTextButton*)editMenuButton->entity)->height}, ((SzTextButton*)editMenuButton->entity)->text, NULL);
            DrawTextButton((SzTextButton*)editMenuButton->entity);
            DrawTextButton((SzTextButton*)aboutMenuButton->entity);

            /*float textWidth1 = MeasureTextEx(*(Font*)buttonDefaultFont->entity, "File", (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            
            Rectangle fileButtonRect = (Rectangle){
                .x = 0 + TEST_TEXT_MARGIN,
                .y = 0,
                .width = textWidth1,
                .height = 30
            };

            GuiLabelButton(fileButtonRect, "File", NULL);
            
            float textWidth2 = MeasureTextEx(*(Font*)buttonDefaultFont->entity, "Edit", (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            
            Rectangle editButtonRect = (Rectangle){
                .x = fileButtonRect.x + fileButtonRect.width + TEST_TEXT_MARGIN,
                .y = 0,
                .width = textWidth2,
                .height = 30
            };
            
            GuiLabelButton(editButtonRect, "Edit", NULL);
            
            float textWidth3 = MeasureTextEx(*(Font*)buttonDefaultFont->entity, "About", (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            
            Rectangle aboutButtonRect = (Rectangle){
                .x = editButtonRect.x + editButtonRect.width + TEST_TEXT_MARGIN,
                .y = 0,
                .width = textWidth3,
                .height = 30
            };
            
            GuiLabelButton(aboutButtonRect, "About", NULL);*/
            DrawRectangleRec(*(Rectangle*)sceneMenu->entity, ORANGE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    DestroyGroup("free_at_shutdown");

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
