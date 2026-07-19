#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <semaphore.h>

typedef enum {
    TASK_DONE,
    TASK_WAIT
} SzTaskState;

typedef enum {
    READER_FORWARD,
    READER_BACKWARD
} SzReaderMode;

typedef struct _SzTask {
    int (*content)(void);
    struct _SzTask* next;
    SzTaskState state;
} SzTask;

typedef struct {
    pthread_t* thread;
    sem_t movement;
    FILE* script;
    SzReaderMode mode;
} SzReader;

// Event-type pair
typedef struct _SzETPair {
    bool (*rlEventFunction)(void);
    void* address;
    struct _SzETPair* next;
} SzETPair;

typedef enum {
    VAR_CLASS_CONFIG,
    VAR_CLASS_LUA,
    VAR_CLASS_SUNRAYZ
} SzVariableClass;

typedef enum {
    VAR_TYPE_INTEGER,
    VAR_TYPE_FLOAT,
    VAR_TYPE_DOUBLE,
    VAR_TYPE_BOOLEAN,
    VAR_TYPE_STRING
} SzVariableType;

typedef struct _SzVariable {
    char* name;
    void* value;
    SzVariableType type;
    SzVariableClass class;
    struct _SzVariable* previous;
    struct _SzVariable* next;
} SzVariable;

typedef struct {
    pthread_t* thread;
    SzETPair* events;
    SzVariable* vars;
} SzActor;

SzTask* CreateTask(int (*taskRoutine)(void));
SzReader* CreateReader(char* script);
SzActor* CreateActor(void);
SzVariable* CreateIntVariable(char* varName, int varValue, SzVariableClass varClass);
SzVariable* CreateFloatVariable(char* varName, float varValue, SzVariableClass varClass);
SzVariable* CreateDoubleVariable(char* varName, double varValue, SzVariableClass varClass);
SzVariable* CreateBoolVariable(char* varName, bool varValue, SzVariableClass varClass);
SzVariable* CreateStringVariable(char* varName, const char* varValue, SzVariableClass varClass);
void DestroyVariable(SzVariable* firstPointer, char* varName);
