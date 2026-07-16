#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
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
    VAR_TYPE_LUA
} SzVariableType;

typedef struct _SzVariable {
    char* name;
    void* value;
    SzVariableType type;
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
