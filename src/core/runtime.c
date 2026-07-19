#include "runtime.h"

#include <raylib.h>
#include <semaphore.h>
#include <stdlib.h>

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

SzVariable* CreateIntVariable(char* varName, int varValue, SzVariableClass varClass) {
    SzVariable* ptr = (SzVariable*)MemAlloc(sizeof(SzVariable));
    int* valuePtr = (int*)MemAlloc(sizeof(int));
    *valuePtr = varValue;
    *ptr = (SzVariable){
        .name = varName,
        .value = valuePtr,
        .type = VAR_TYPE_INTEGER,
        .class = varClass,
        .previous = NULL,
        .next = NULL
    };
    return ptr;
}

SzVariable* CreateFloatVariable(char* varName, float varValue, SzVariableClass varClass) {
    SzVariable* ptr = (SzVariable*)MemAlloc(sizeof(SzVariable));
    float* valuePtr = (float*)MemAlloc(sizeof(float));
    *valuePtr = varValue;
    *ptr = (SzVariable){
        .name = varName,
        .value = valuePtr,
        .type = VAR_TYPE_FLOAT,
        .class = varClass,
        .previous = NULL,
        .next = NULL
    };
    return ptr;
}

SzVariable* CreateBoolVariable(char* varName, bool varValue, SzVariableClass varClass) {
    SzVariable* ptr = (SzVariable*)MemAlloc(sizeof(SzVariable));
    bool* valuePtr = (bool*)MemAlloc(sizeof(bool));
    *valuePtr = varValue;
    *ptr = (SzVariable){
        .name = varName,
        .value = valuePtr,
        .type = VAR_TYPE_BOOLEAN,
        .class = varClass,
        .previous = NULL,
        .next = NULL
    };
    return ptr;
}

SzVariable* CreateStringVariable(char* varName, const char* varValue, SzVariableClass varClass) {
    SzVariable* ptr = (SzVariable*)MemAlloc(sizeof(SzVariable));
    *ptr = (SzVariable){
        .name = varName,
        .value = (char*)varValue,
        .type = VAR_TYPE_STRING,
        .class = varClass,
        .previous = NULL,
        .next = NULL
    };
    return ptr;
}

/// Note: expects a pointer to the first variable of a sequence
/// TODO: Potentially buggy; will segfault if unable to access the previous variable
void DestroyVariable(SzVariable* firstPointer, char* varName) {
    SzVariable* current = firstPointer;
    SzVariable* previous = NULL;
    while (current->name != varName) {
        previous = current;
        current = current->next;
    }
    if (current->next != NULL) {
        previous->next = current->next;
        MemFree(current);
    }
}