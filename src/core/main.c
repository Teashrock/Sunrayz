#include <raylib.h>
#include <raygui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luajit.h>

#include "base.h"
#include "queue.h"

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

int testTask(void) {
    ClearBackground(WHITE);

    return 0;
}

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

    // Initializing Lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Sunrayz");
    SetWindowMinSize(screenWidth, screenHeight);
    SetWindowSize(screenWidth, screenHeight);
    MaximizeWindow();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    bool* dbg = malloc(sizeof(bool));
    *dbg = false;

    // Lua script implementation
    char* scriptDirArray[] = {GetApplicationDirectory(), "system"};
    char* scriptDir = TextJoin(scriptDirArray, 2, pathDelimiter);
    if (!DirectoryExists(scriptDir)) {
        // Here be code for queueing an error message
    }

    // Test area
    SzQueue* test_queue = CreateQueue();
    CreateTask(test_queue, testTask);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            test_queue->state = QUEUE_RUNNING;

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    FlushQueue(test_queue);
    lua_close(L);

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
