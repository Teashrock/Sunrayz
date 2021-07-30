#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"
#include "error.h"

const int uniValues[] = {
    80 // Menu and panel collision point
};

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    #define screenWidth  800
    #define screenHeight 600

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Sunrayz");
    SetWindowMinSize(screenWidth, screenHeight);
    SetWindowSize(screenWidth, screenHeight);
    MaximizeWindow();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    Rectangle panel = {0, 0,            DYN_WIDTH, uniValues[0]};
    Rectangle menu  = {0, uniValues[0], 260,       DYN_HEIGHT};
    
    Font calibri = CreateFont("calibri.ttf", 28, 95);
    TextButton* fileMenuButton = CreateButton(
        "File",
        0, 0,
        100, uniValues[0],
        WHITE,
        LIGHTGRAY,
        GREEN,
        &calibri,
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
            DrawButton(fileMenuButton, false);
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
