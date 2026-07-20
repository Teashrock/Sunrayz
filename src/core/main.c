#include <raylib.h>
#include <raygui.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#ifdef __HAIKU__
#include <stdlib.h>
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luajit.h>
#include <string.h>

#include "base.h"
#include "config.h"
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
    int screenWidth = 0; int screenHeight = 0; int windowResizable = 0; int maximizeWindow = 0; int fullscreen = 0;
    char* cfgFileName = TextJoin((char*[]){(char*)GetApplicationDirectory(), "settings.cfg"}, 2, pathDelimiter);
    SzConfig* cfg;
    if (FileExists(cfgFileName)) {
        cfg = ReadConfig(cfgFileName);
        screenWidth = *(int*)GetConfigVariable(cfg, "screenWidth")->value;
        screenHeight = *(int*)GetConfigVariable(cfg, "screenHeight")->value;
        windowResizable = *(bool*)GetConfigVariable(cfg, "windowResizable")->value;
        maximizeWindow = *(bool*)GetConfigVariable(cfg, "maximizeWindow")->value;
        fullscreen = *(bool*)GetConfigVariable(cfg, "fullscreen")->value;
    } else {
        cfg = CreateConfig("Sunrayz");
        AddVariable(
            &cfg->variables,
            CreateIntVariable("screenWidth", SCREEN_WIDTH_DEFAULT, VAR_CLASS_SUNRAYZ)
        );
        screenWidth = *(int*)GetConfigVariable(cfg, "screenWidth")->value;
        AddVariable(
            &cfg->variables,
            CreateIntVariable("screenHeight", SCREEN_HEIGHT_DEFAULT, VAR_CLASS_SUNRAYZ)
        );
        screenHeight = *(int*)GetConfigVariable(cfg, "screenHeight")->value;
        AddVariable(
            &cfg->variables,
            CreateBoolVariable("windowResizable", true, VAR_CLASS_SUNRAYZ)
        );
        windowResizable = *(bool*)GetConfigVariable(cfg, "windowResizable")->value;
        AddVariable(
            &cfg->variables,
            CreateBoolVariable("maximizeWindow", false, VAR_CLASS_SUNRAYZ)
        );
        maximizeWindow = *(bool*)GetConfigVariable(cfg, "maximizeWindow")->value;
        AddVariable(
            &cfg->variables,
            CreateBoolVariable("fullscreen", false, VAR_CLASS_SUNRAYZ)
        );
        fullscreen = *(bool*)GetConfigVariable(cfg, "fullscreen")->value;
        WriteConfig(cfgFileName, cfg);
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
    if (fullscreen)
        ToggleFullscreen();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Lua script implementation
    char* scriptDir = TextJoin(
        (char*[]){(char*)GetApplicationDirectory(), "scripts"},
        2,
        pathDelimiter
    );
    
    // Listing lua files
    FilePathList luaFiles = LoadDirectoryFilesEx(scriptDir, ".lua", true);
    for (int i = 0; i < luaFiles.count; i++) {
        if (!strcmp(GetFileName(luaFiles.paths[i]), "start.lua")) { // Right now, start.lua is our starting file
            luaL_loadfile(L, luaFiles.paths[i]);
            ChangeDirectory(scriptDir);
            if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
                printf("Lua error: %s\n", lua_tostring(L, -1));
        }
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
