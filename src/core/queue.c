#include "queue.h"

#include <pthread.h>
#include <raylib.h>

void CreateTask(SzQueue* q, int (* taskRoutine)(void)) {
    SzTask* ptr = (SzTask*)MemAlloc(sizeof(SzTask));
    *ptr = (SzTask){
        .content = taskRoutine,
        .state = TASK_NEW,
        .next = NULL
    };
    SzTask* roll = q->tasks;
    while (ptr != NULL) {
        ptr = ptr->next;
    }
    while (ptr->state == TASK_NEW); // Standing by
    if (ptr->content() == 0)
        ptr->state = TASK_DONE;
}

void* QueueRoutine(void* arg) {
    SzQueue* q = (SzQueue*)arg;
    SzTask* currentTask = q->tasks;
    while (true) {
        while (q->state == QUEUE_STANDBY); // Doing nothing while the queue is standing by
        currentTask->state = TASK_READY;
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

void FlushQueue(SzQueue* q) {
    q->state = QUEUE_STANDBY;
    pthread_join(*(q->thread), NULL);
    q->thread = NULL;
    SzTask* curr_task = q->tasks;
    while (curr_task != NULL) {
        SzTask* next_task = curr_task->next;
        MemFree(curr_task);
        SzTask* curr_task = next_task;
    }
    q->tasks = NULL;
    MemFree(q);
}