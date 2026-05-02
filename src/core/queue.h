#pragma once

#include <pthread.h>
#include <stddef.h>
#include <semaphore.h>

typedef enum {
    TASK_DONE,
    TASK_NEW,
    TASK_PREPARING,
    TASK_READY
} SzTaskState;

typedef enum {
    QUEUE_CLEANING,
    QUEUE_RUNNING,
    QUEUE_STANDBY
} SzQueueState;

typedef struct _SzTask {
    int (*content)(void);
    struct _SzTask* next;
    SzTaskState state;
} SzTask;

typedef struct _SzQueue {
    pthread_t* thread;
    sem_t movement;
    SzQueueState state;
    int task_limit;
    SzTask* tasks;
} SzQueue;

void CreateTask(SzQueue* assignTo, int (* taskRoutine)(void));
SzQueue* CreateQueue(void);
void FlushQueue(SzQueue*);
