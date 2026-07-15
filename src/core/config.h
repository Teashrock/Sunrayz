#pragma once

typedef struct _SzParameter {
    char* name;
    int value;
    struct _SzParameter* next;
} SzParameter;

typedef struct _SzConfig {
    char* name;
    SzParameter* parameters;
    struct _SzConfig* next;
} SzConfig;

SzConfig* ReadConfig(char* cfgName);
void WriteConfig(char* cfgName, SzConfig* section);