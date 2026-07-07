#include <raylib.h>
#include <raygui.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>

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
    #define SCREEN_WIDTH_DEFAULT 1280
    #define SCREEN_HEIGHT_DEFAULT 1024
    
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

    // Initializing main config
    int screenWidth, screenHeight, windowResizable, maximizeWindow, fullscreen;
    char currentCfgSection[5];
    char* cfgName = TextJoin((char*[]){(char*)GetApplicationDirectory(), "settings.cfg"}, 2, pathDelimiter);
    if (FileExists(cfgName)) {
        FILE* cfg = fopen(cfgName, "r");
        fscanf(cfg, "[%4s]\n", currentCfgSection);
        //fseek(cfg, 7, SEEK_CUR);
        fscanf(cfg, "screenWidth=%d\n", &screenWidth);
        fscanf(cfg, "screenHeight=%d\n", &screenHeight);
        fscanf(cfg, "windowResizable=%d\n", &windowResizable);
        fscanf(cfg, "maximizeWindow=%d\n", &maximizeWindow);
        fscanf(cfg, "fullscreen=%d\n", &fullscreen);
        fclose(cfg);
    } else {
        screenHeight = SCREEN_HEIGHT_DEFAULT;
        screenWidth = SCREEN_WIDTH_DEFAULT;
        FILE* cfg = fopen(cfgName, "wt");
        fprintf(cfg, "[Main]\n");
        fprintf(cfg, "screenWidth=%d\n", SCREEN_WIDTH_DEFAULT);
        fprintf(cfg, "screenHeight=%d\n", SCREEN_HEIGHT_DEFAULT);
        fprintf(cfg, "windowResizable=1\n");
        fprintf(cfg, "maximizeWindow=0\n");
        fprintf(cfg, "fullscreen=0\n");
        fclose(cfg);
    }

    // Initializing Lua
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (windowResizable)
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Sunrayz");
    SetWindowMinSize(screenWidth, screenHeight);
    SetWindowSize(screenWidth, screenHeight);
    if (maximizeWindow)
        MaximizeWindow();
    //if (!fullscreen)
    //    ToggleFullscreen();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Lua script implementation
    char* scriptDir = TextJoin(
        (char*[]){(char*)GetApplicationDirectory(), "system"},
        2,
        pathDelimiter
    );
    
    // Listing lua files
    FilePathList luaFiles = LoadDirectoryFilesEx(scriptDir, ".lua", true);
    for (int i = 0; i < luaFiles.count; i++) {
        char* pathPtr = (char*)MemAlloc(sizeof(char) * (strlen(luaFiles.paths[i]) + 1));
        char* pathIter = pathPtr;
        char* token = NULL;
        strcpy(pathIter, luaFiles.paths[i]);
        while ((token = strtok(pathIter, pathDelimiter)) != NULL) {
            if (pathIter != NULL) {
                pathIter = NULL;
            }
            if (!strcmp(token, "start.lua")) { // Right now, start.lua is our starting file
                luaL_loadfile(L, luaFiles.paths[i]);
                ChangeDirectory(scriptDir);
                if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
                    printf("Lua error: %s\n", lua_tostring(L, -1));
            }
        };
        MemFree(pathPtr);
        pathPtr = NULL;
    }

    // Test area
    //SzReader* testReader = CreateReader("");
    SzActor* actor = CreateActor();
    SzTask* task = CreateTask(testTask);

    // Main game loop
    /*while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            // An empty place

        EndDrawing();
        //----------------------------------------------------------------------------------
    }*/

    // De-Initialization
    lua_close(L);

    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
