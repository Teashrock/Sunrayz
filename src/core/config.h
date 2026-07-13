#pragma once

typedef struct _SzParameter {
    char* name;
    int value;
    struct _SzParameter* next;
} SzParameter;

typedef struct {
    char* name;
    SzParameter* parameters;
} SzConfig;

void ReadConfig(char* cfgName);
void WriteConfig(char* cfgName);