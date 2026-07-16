#include "config.h"
#include "runtime.h"

#include <raylib.h>
#include <stdbool.h>
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

/// Checks if a string can be seamlessly converted into integer.
/// Warning: expects a zstring.
bool IsInteger(char* string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!(string[i] >= '0' && string[i] <= '9'))
            return false;
    }
    return true;
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
                SzVariable* param = (SzVariable*)MemAlloc(sizeof(SzVariable));
                param->next = NULL;
                ReadToString(cfg, '=', param->name);
                fseek(cfg, 1, SEEK_CUR);
                // Saving the value to a temporary variable
                char* tmpValue;
                ReadToString(cfg, '\n', tmpValue);
                // Determining the type of variable to save it properly
                if (IsInteger(tmpValue)) {
                    int* newValue = (int*)MemAlloc(sizeof(int));
                    *newValue = atoi(tmpValue);
                    MemFree(tmpValue);
                    param->value = newValue;
                    newValue = NULL;
                } else {
                    param->value = tmpValue;
                }
                tmpValue = NULL;
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
        SzVariable* currentParam = currentSection->parameters;
        while (currentParam != NULL) {
            fprintf(
                cfg,
                "%s=%d\n",
                currentParam->name,
                *(int*)currentParam->value
            );
            currentParam = currentParam->next;
        }
        currentSection = currentSection->next;
    }
    fclose(cfg);
}

SzVariable* GetParameter(SzConfig* cfg, char* paramName) {

}