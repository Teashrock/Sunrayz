#include "runtime.h"

#include <raylib.h>
#include <semaphore.h>

SzTask* CreateTask(int (*taskRoutine)(void)) {
    SzTask* ptr = (SzTask*)MemAlloc(sizeof(SzTask));
    *ptr = (SzTask){
        .content = taskRoutine,
        .state = TASK_WAIT,
        .next = NULL
    };
    
    return ptr;
}

void* ReaderRoutine(void* arg) {
    SzReader* ptr = arg;
    return arg;
}

SzReader* CreateReader(char* script) {
    SzReader* ptr = (SzReader*)MemAlloc(sizeof(SzReader));
    *ptr = (SzReader){
        .thread = NULL,
        .script = fopen(script, "r"),
        .mode = READER_FORWARD
    };
    if (!ptr->script) {
        printf("ERROR: Failed to create SzReader: failed to open %s\n", script);
        exit(1);
    }
    pthread_create(ptr->thread, NULL, ReaderRoutine, ptr);
    return ptr;
}

void* ActorRoutine(void* arg) {
    SzActor* ptr = (SzActor*)arg;
}

SzActor* CreateActor(void) {
    SzActor* ptr = (SzActor*)MemAlloc(sizeof(SzActor));
    *ptr = (SzActor){
        .events = NULL,
        .vars = NULL
    };
    pthread_create(ptr->thread, NULL, ReaderRoutine, ptr);
    return ptr;
}