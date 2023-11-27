#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <stddef.h>

#include "base.h"

void SzRuntimeError(char* title, char* text)
{
    if (title == NULL)
        title = "Runtime error!";
    if (text == NULL)
        text = "A runtime error has occured.";
    puts(text);
    DestroyGroup("free_at_shutdown");
    exit(1);
}

void SzDebugInfo(char* text, bool* oneshot)
{
    if (!(*oneshot)) {
        *oneshot = true;
        if (text == NULL)
            text = "A debug message has been called.";
        puts(text);
        exit(1);
    }
}
