#include "config.h"

#include "base.h"
#include "runtime.h"

#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Reads a configuration file into the engine memory
SzConfig* ReadConfig(char* cfgName) {
    FILE* cfg = fopen(cfgName, "rt");
    // Counting chars until a newline
    char c[1];
    int charCounter = 0;
    long filePosition = ftell(cfg);
    SzConfig* section = NULL;
    while (true) {
        int readResult = fread(c, sizeof(char), 1, cfg);
        if (readResult < 1) {
            if (feof(cfg)) {
                fclose(cfg);
                break;
            } else if (ferror(cfg)) {
                printf("Error reading file %s!\n", cfgName);
                exit(1);
            }
        }
        charCounter++;
        filePosition++;
        // When we've found a newline, we return and scan the line
        if (c[0] == '\n') {
            fseek(cfg, filePosition - charCounter, SEEK_SET);
            int readResult = fread(c, sizeof(char), 1, cfg);
            if (readResult != 1) {
                if (feof(cfg)) {
                    fclose(cfg);
                }
            }
            if (c[0] == '[') {
                // An opening square bracket means a section name
                while (section != NULL) {
                    section = section->next;
                }
                section = (SzConfig*)MemAlloc(sizeof(SzConfig));
                section->variables = NULL;
                filePosition = StringReadUntil(cfg, ']', &section->name) + 2;
            } else {
                // Moving one character backwards to capture the name since the first letter
                fseek(cfg, -1, SEEK_CUR);
                // If it's not an opening square bracket, then we're reading a parameter
                SzVariable* param = (SzVariable*)MemAlloc(sizeof(SzVariable));
                param->next = NULL;
                fseek(cfg,  StringReadUntil(cfg, '=', &param->name) + 1, SEEK_SET);
                // Saving the value to a temporary variable
                char* tmpValue;
                filePosition = StringReadUntil(cfg, '\n', &tmpValue) + 1;
                // Determining the type of variable to save it properly
                if (StringIsInteger(tmpValue)) {
                    int* newValue = (int*)MemAlloc(sizeof(int));
                    *newValue = atoi(tmpValue);
                    MemFree(tmpValue);
                    param->value = newValue;
                    newValue = NULL;
                } else if (StringIsFraction(tmpValue)) {
                    printf("tmpValue: %s\n", tmpValue);
                    // We're using float because double is overkill for a config variable
                    float* newValue = (float*)MemAlloc(sizeof(float));
                    *newValue = atof(tmpValue);
                    MemFree(tmpValue);
                    param->value = newValue;
                    newValue = NULL;
                } else if (StringIsBool(tmpValue)) {
                    bool* newValue = (bool*)MemAlloc(sizeof(bool));
                    if (!strcmp(tmpValue, "true"))
                        *newValue = true;
                    else if (!strcmp(tmpValue, "false"))
                        *newValue = false;
                    MemFree(tmpValue);
                    param->value = newValue;
                    newValue = NULL;
                } else {
                    param->value = tmpValue;
                }
                tmpValue = NULL;
                AddConfigVariable(section, param);
            }
            charCounter = 0;
        }
        fseek(cfg, filePosition, SEEK_SET);
    }
    return section;
}

/// Writes variables from memory into config file
void WriteConfig(char* cfgName, SzConfig* section) {
    FILE* cfg = fopen(cfgName, "wt");
    SzConfig* currentSection = section;
    while (currentSection != NULL) {
        fprintf(cfg, "[%s]\n", currentSection->name);
        SzVariable* currentVar = currentSection->variables;
        while (currentVar != NULL) {
            if (currentVar->type == VAR_TYPE_BOOLEAN || currentVar->type == VAR_TYPE_STRING) {
                char* boolString = NULL;
                if (currentVar->type == VAR_TYPE_BOOLEAN) {
                    if (*(bool*)currentVar->value)
                        boolString = "true";
                    else
                        boolString = "false";
                }
                fprintf(
                    cfg,
                    "%s=%s\n",
                    currentVar->name,
                    boolString
                );
            } else if (currentVar->type == VAR_TYPE_FLOAT || currentVar->type == VAR_TYPE_DOUBLE) {
                fprintf(
                    cfg,
                    "%s=%f\n",
                    currentVar->name,
                    *(double*)currentVar->value
                );
            } else {
                fprintf(
                    cfg,
                    "%s=%d\n",
                    currentVar->name,
                    *(int*)currentVar->value
                );
            }
            currentVar = currentVar->next;
        }
        currentSection = currentSection->next;
    }
    fclose(cfg);
}

SzConfig* CreateConfig(char* cfgName) {
    SzConfig* ptr = (SzConfig*)MemAlloc(sizeof(SzConfig));
    *ptr = (SzConfig){
        .name = cfgName,
        .variables = NULL,
        .next = NULL
    };
    return ptr;
}

SzVariable* GetConfigVariable(SzConfig* cfg, char* varName) {
    SzVariable* var = cfg->variables;
    while (var != NULL) {
        if (!strcmp(var->name, varName)) {
            return var;
        }
        var = var->next;
    }
    printf("Variable error: %s wasn't found!\n", varName);
    return NULL;
}

void AddConfigVariable(SzConfig* cfg, SzVariable* var) {
    if (cfg->variables == NULL) {
        cfg->variables = var;
    } else {
        SzVariable* current = NULL;
        SzVariable* last = cfg->variables;
        while (last != NULL) {
            current = last;
            last = last->next;
        }
        current->next = var;
        var->previous = current;
    }
}