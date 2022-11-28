#ifdef _WIN32
//#include <windows.h>
#elif __linux__
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#endif

#include "memory.h"

void SzRuntimeError(char* title, char* text)
{
    if (title == NULL)
        title = "Runtime error!";
    if (text == NULL)
        text = "A runtime error has occured.";
    #ifdef _WIN32
    //MessageBox(NULL, (LPCTSTR)text, (LPCTSTR)title, MB_OK | MB_ICONERROR);
    #elif __linux__
    puts(text);
    #endif
    DestroyGroup("free_at_shutdown");
    #ifdef __linux__
    exit(1);
    #endif
}

void SzDebugInfo(char* text)
{
    if (text == NULL)
        text = "A debug message has been called.";
    #ifdef _WIN32
    //MessageBox(NULL, (LPCTSTR)text, (LPCTSTR)"Debug Information", MB_OK | MB_ICONINFORMATION);
    #elif __linux__
    puts(text);
    exit(1);
    #endif
}
