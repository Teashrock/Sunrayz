#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <locale.h>

#include "gui.h"
#include "error.h"
#include "memory.h"

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

    SzType* menuPanel = CreateRec(0, 0, GetScreenWidth(), uniValues[0]);
    SzType* sceneMenu  = CreateRec(0, uniValues[0], 260, GetScreenHeight());

    SzType* buttonDefaultFont = CreateFont(
        GetFontLibertinusSerif_Regular(),
        GetFontLibertinusSerif_RegularSize(),
        24,
        2048,
        FONT_CREATION_METHOD_RAW
    );
	//GuiSetFont(*((Font*)buttonDefaultFont->entity));
    
    /*SzType* fileMenuButton = CreateTextButton(
        "File",
        0, 0,
        45, uniValues[0],
        WHITE,
        LIGHTGRAY,
        GREEN,
        (Font*)buttonDefaultFont->entity,
        NULL,
        30,
        BLACK,
        BLACK,
        BLACK,
        5,
        2.1
    );*/

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GRAY);

            DrawRectangleRec(*(Rectangle*)menuPanel->entity, WHITE);
            //GuiButton((Rectangle){0, 0, 45, 30}, "File");
            //GuiButton((Rectangle){45, 0, 45, 30}, "Edit");
            //GuiButton((Rectangle){90, 0, 65, 30}, "About");
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
