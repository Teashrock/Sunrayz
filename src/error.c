#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#endif

void SzError(char* title, char* text)
{
    if (title == NULL)
        title = "Runtime error!";
    if (text == NULL)
        text = "A runtime error has occured.";
    #ifdef _WIN32
    MessageBox(NULL, (LPCTSTR)text, (LPCTSTR)title, MB_OK | MB_ICONERROR);
    #elif __linux__
    puts(text);
    exit(1);
    #endif
}
