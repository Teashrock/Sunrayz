#pragma once

#include <pthread.h>

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
    struct _SzTask* next;
    SzTaskState state;
} SzTask;

typedef struct _SzQueue {
    pthread_t* thread;
    SzQueueState state;
    int task_limit;
    SzTask* tasks;
} SzQueue;

SzQueue* CreateQueue(void);
