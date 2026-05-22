#pragma once

#include <pthread.h>
#include <stddef.h>
#include <semaphore.h>

typedef enum {
    TASK_DONE,
    TASK_WAIT
} SzTaskState;

typedef struct _SzTask {
    int (*content)(void);
    struct _SzTask* next;
    SzTaskState state;
} SzTask;

void CreateTask(int (* taskRoutine)(void));
