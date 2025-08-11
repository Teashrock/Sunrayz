//#include <raygui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <kuroko/kuroko.h>
#include <kuroko/vm.h>

#include "base.h"
#include "error.h"
#include "gui.h"

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

    // Initializing Kuroko
    krk_initVM(0);
    krk_startModule("__main__");

    rl_SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    rl_InitWindow(screenWidth, screenHeight, "Sunrayz");
    rl_SetWindowMinSize(screenWidth, screenHeight);
    rl_SetWindowSize(screenWidth, screenHeight);
    rl_MaximizeWindow();
    rl_SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    srand(time(NULL));
    CreateGroup("free_at_shutdown", "Rectangle,TextButton,Font,Void");

    SzConstruct* menuPanel = CreateConstruct(NULL);
    SzConstruct* panel1    = CreateConstruct(menuPanel);

    SzEntity* panel = CreateRec(0, 0, rl_GetScreenWidth(), uniValues[0]);
    AddEntity(menuPanel, panel);
    SzEntity* sceneMenu  = CreateRec(0, uniValues[0], 260, rl_GetScreenHeight());

    SzEntity* buttonDefaultFont = CreateFont(
        GetFontLibertinusSerif_Regular(),
        GetFontLibertinusSerif_RegularSize(),
        24,
        2048,
        FONT_CREATION_METHOD_RAW
    );
	rl_GuiSetFont(*((Rl_Font*)buttonDefaultFont->essence));
            
    #define TEST_TEXT_MARGIN 5
    
    SzEntity* fileMenuButton = CreateTextButton(
        "File",
        0, 0,
        45, 30,
        TEST_TEXT_MARGIN / 2,
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
        ((SzTextButton*)fileMenuButton->essence)->posX + ((SzTextButton*)fileMenuButton->essence)->width, 0,
        45, 30,
        TEST_TEXT_MARGIN / 2,
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
        ((SzTextButton*)editMenuButton->essence)->posX + ((SzTextButton*)editMenuButton->essence)->width, 0,
        45, 30,
        TEST_TEXT_MARGIN / 2,
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
    
    bool* dbg = malloc(sizeof(bool));
    *dbg = false;

    // Main game loop
    while (!rl_WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        rl_BeginDrawing();

            rl_ClearBackground(WHITE);

            rl_DrawRectangleRec(*(Rl_Rectangle*)panel->essence, WHITE);

            DrawTextButton((SzTextButton*)fileMenuButton->essence);
            //GuiLabelButton((Rectangle){((SzTextButton*)fileMenuButton->essence)->posX + ((SzTextButton*)fileMenuButton->essence)->margin, ((SzTextButton*)fileMenuButton->essence)->posY, ((SzTextButton*)fileMenuButton->essence)->width, ((SzTextButton*)fileMenuButton->essence)->height}, ((SzTextButton*)fileMenuButton->essence)->text, NULL);
            DrawTextButton((SzTextButton*)editMenuButton->essence);
            //GuiLabelButton((Rectangle){((SzTextButton*)editMenuButton->essence)->posX + ((SzTextButton*)editMenuButton->essence)->margin, ((SzTextButton*)editMenuButton->essence)->posY, ((SzTextButton*)editMenuButton->essence)->width, ((SzTextButton*)editMenuButton->essence)->height}, ((SzTextButton*)editMenuButton->essence)->text, NULL);
            DrawTextButton((SzTextButton*)aboutMenuButton->essence);

            /*float textWidth1 = MeasureTextEx(*(Font*)buttonDefaultFont->essence, "File", (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            
            Rectangle fileButtonRect = (Rectangle){
                .x = 0 + TEST_TEXT_MARGIN,
                .y = 30,
                .width = textWidth1,
                .height = 30
            };

            GuiLabelButton(fileButtonRect, "File", NULL);
            
            float textWidth2 = MeasureTextEx(*(Font*)buttonDefaultFont->essence, "Edit", (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            
            Rectangle editButtonRect = (Rectangle){
                .x = fileButtonRect.x + fileButtonRect.width + TEST_TEXT_MARGIN,
                .y = 30,
                .width = textWidth2,
                .height = 30
            };
            
            GuiLabelButton(editButtonRect, "Edit", NULL);
            
            float textWidth3 = MeasureTextEx(*(Font*)buttonDefaultFont->essence, "About", (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
            
            Rectangle aboutButtonRect = (Rectangle){
                .x = editButtonRect.x + editButtonRect.width + TEST_TEXT_MARGIN,
                .y = 30,
                .width = textWidth3,
                .height = 30
            };
            
            GuiLabelButton(aboutButtonRect, "About", NULL);
            //DrawRectangleRec(*(Rectangle*)sceneMenu->essence, ORANGE);*/

        rl_EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    DestroyGroup("free_at_shutdown");
    krk_freeVM();

    //--------------------------------------------------------------------------------------
    rl_CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
