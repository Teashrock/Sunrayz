#pragma once

typedef struct _SzTask {

} SzTask;

typedef struct _SzQueue {
    SzTask** tasks;
} SzQueue;

int CreateQueue();
