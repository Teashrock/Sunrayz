#pragma once

#include "runtime.h"

typedef struct _SzConfig {
    char* name;
    SzVariable* variables;
    struct _SzConfig* next;
} SzConfig;

SzConfig* ReadConfig(char* cfgName);
void WriteConfig(char* cfgName, SzConfig* section);
SzVariable* GetConfigVariable(SzConfig* cfg, char* varName);