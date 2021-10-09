#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui.h"
#include "error.h"

const int uniValues[] = {
    80 // Menu and panel collision point
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
#else
static char* APP_LOC = NULL;
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
    
    Rectangle panel = {0, 0,            DYN_WIDTH, uniValues[0]};
    Rectangle menu  = {0, uniValues[0], 260,       DYN_HEIGHT};
    
    Font* calibri = CreateFont("calibri.ttf", 28, 95);
    TextButton* fileMenuButton = CreateButton(
        "File",
        0, 0,
        100, uniValues[0],
        WHITE,
        LIGHTGRAY,
        GREEN,
        calibri,
        NULL,
        68,
        BLACK,
        BLACK,
        BLACK
    );

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GRAY);

            DrawRectangleRec(panel, WHITE);
            UpdateRec(&panel);
            DrawButton(fileMenuButton, true);
            DrawRectangleRec(menu, ORANGE);
            UpdateRec(&menu);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    FreeFreeables();

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
