#pragma once

/*typedef enum {
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_BOOL,
    TOKEN_FUNCTION,
    TOKEN_TYPE
} ValType;*/

/*typedef struct _Signal
{
    char* name;
    Condition* conditions;
    Signal* next;
} Signal;*/

typedef struct _SzType
{
    void* entity;
    char* type;
    struct _SzType* next;
    int* id;
} SzType;

//int CreateSignal(SzVisible* object, char* name);
SzType* CreateType(char* typeName);