#include "queue.h"

#include <pthread.h>
#include <raylib.h>
#include <semaphore.h>

void CreateTask(int (* taskRoutine)(void)) {
    SzTask* ptr = (SzTask*)MemAlloc(sizeof(SzTask));
    *ptr = (SzTask){
        .content = taskRoutine,
        .state = TASK_WAIT,
        .next = NULL
    };
}