#include "queue.h"

#include <pthread.h>
#include <raylib.h>

SzTask* CreateTask(SzQueue* q) {
    SzTask* ptr = q->tasks;
    while (ptr != NULL) {
        ptr = ptr->next;
    }
    ptr = (SzTask*)MemAlloc(sizeof(SzTask));
    *ptr = (SzTask){
        .content = NULL,
        .state = TASK_NEW,
        .next = NULL
    };
    q->state = QUEUE_RUNNING;
}

void* QueueRoutine(void* arg) {
    SzQueue* q = (SzQueue*)arg;
    SzTask* currentTask = q->tasks;
    while (true) {
        while (q->state == QUEUE_STANDBY); // Doing nothing while the queue is standing by
        //currentTask
    }
}

SzQueue* CreateQueue(void) {
    SzQueue* ptr = (SzQueue*)MemAlloc(sizeof(SzQueue));

    *ptr = (SzQueue){
        .thread = NULL,
        .state = QUEUE_STANDBY,
        .tasks = NULL
    };
    pthread_create(ptr->thread, NULL, QueueRoutine, ptr);

    return ptr;
}