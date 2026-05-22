#pragma once

#include <pthread.h>
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

typedef struct _SzReader {
    pthread_t* thread;
    sem_t movement;
    FILE* script;
    SzReaderMode mode;
} SzReader;

void CreateTask(int (* taskRoutine)(void));
SzReader* CreateReader(char* script);
