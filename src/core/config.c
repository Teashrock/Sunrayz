#include "config.h"

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Reads a string from an opened file
/// up to the provided character (char until),
/// then writes it by a provided char* pointer (dest).
/// No need to allocate dest in advance.
void ReadToString(FILE* f, char until, char* dest) {
    // Counting chars until the specific symbol is met
    char c[1];
    int charCount = 0;
    while (true) {
        fread(c, sizeof(char), 1, f);
        if (c[0] == until) {
            fseek(f, -(charCount + 1), SEEK_CUR);
            break;
        }
        charCount++;
    }
    // Writing the string found into the destination
    dest = (char*)MemAlloc(sizeof(char) * (charCount + 1));
    fread(dest, sizeof(char), charCount, f);
    dest[charCount + 1] = '\0';
}

/// Reads a configuration file into the engine memory
SzConfig* ReadConfig(char* cfgName) {
    FILE* cfg = fopen(cfgName, "rt");
    // Counting chars until a newline
    char c[1];
    int charCounter = 0;
    SzConfig* section;
    while (true) {
        int readResult = fread(c, sizeof(char), 1, cfg);
        if (readResult != 1) {
            if (feof(cfg))
                break;
            else if (ferror(cfg))
                {}
        }
        charCounter++;
        // When we've found a newline, we return and scan the line
        if (c[0] == '\n') {
            fseek(cfg, -(charCounter + 1), SEEK_CUR);
            fread(c, sizeof(char), 1, cfg);
            if (c[0] == '[') {
                // An opening square bracket means a section name
                while (section != NULL) {
                    section = section->next;
                }
                section = (SzConfig*)MemAlloc(sizeof(SzConfig));
                section->parameters = NULL;
                ReadToString(cfg, ']', section->name);
                fseek(cfg, 2, SEEK_CUR);
            } else {
                // If it's not a opening square bracket, then we're reading a parameter
                SzParameter* param = (SzParameter*)MemAlloc(sizeof(SzParameter));
                param->next = NULL;
                ReadToString(cfg, '=', param->name);
                fseek(cfg, 1, SEEK_CUR);
                // Saving the value to a temporary variable
                char* tmpValue;
                ReadToString(cfg, '\n', tmpValue);
                // Determining the type of variable to save it properly
                if (!strcmp(tmpValue, "true")) { // Booleans go first
                    param->value = 1;
                } else if (!strcmp(tmpValue, "false")) {
                    param->value = 0;
                } else { // Assuming the value is an integer (TODO: implement a proper check)
                    param->value = atoi(tmpValue);
                }
            }
        }
    }
    return section;
}

void WriteConfig(char* cfgName, SzConfig* section) {
    FILE* cfg = fopen(cfgName, "wt");
    SzConfig* currentSection = section;
    while (currentSection != NULL) {
        fprintf(cfg, "[%s]\n", currentSection->name);
        SzParameter* currentParam = currentSection->parameters;
        while (currentParam != NULL) {
            fprintf(
                cfg,
                "%s=%d\n",
                currentParam->name,
                currentParam->value
            );
            currentParam = currentParam->next;
        }
        currentSection = currentSection->next;
    }
    fclose(cfg);
}