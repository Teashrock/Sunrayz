#include <raylib.h>
#include <raygui.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luajit.h>
#include <string.h>

#include "base.h"
#include "runtime.h"

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
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Sunrayz");
    SetWindowMinSize(screenWidth, screenHeight);
    SetWindowSize(screenWidth, screenHeight);
    MaximizeWindow();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Lua script implementation
    char* scriptDirArray[] = {(char*)GetApplicationDirectory(), "system"};
    char* scriptDir = TextJoin(scriptDirArray, 2, pathDelimiter);
    /* if (!DirectoryExists(scriptDir)) {
        printf("ERROR: Directory \"%s\" was not found!", scriptDir);
        exit(1);
    } */
    
    // Listing lua files
    FilePathList luaFiles = LoadDirectoryFilesEx(scriptDir, ".lua", true);
    for (int i = 0; i < luaFiles.count; i++) {
        char* path = (char*)MemAlloc(sizeof(strlen(luaFiles.paths[i])));
        char* token = NULL;
        strcpy(path, luaFiles.paths[i]);
        while (!(token = strtok(path, pathDelimiter))) {
            if (path != NULL)
                path = NULL;
            printf("%s\n", token);
            if (strcmp(token, "first.lua")) // Right now, first.lua is our starting file
                luaL_loadfile(L, luaFiles.paths[i]);
        };
        MemFree(path);
        token = NULL;
        path = NULL;
    }

    // Test area
    //SzReader* testReader = CreateReader("");
    SzActor* actor = CreateActor();
    SzTask* task = CreateTask(testTask);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            //sem_post(testReader->movement);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    lua_close(L);

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
