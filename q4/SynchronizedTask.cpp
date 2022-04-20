#include "SynchronizedTask.h"
#include <cstdio>

static Semaphore sem(1);

SynchronizedTask::SynchronizedTask(std::function<void()> t) {
    task = t;
}

void SynchronizedTask::execute() {
    sem.acquire();
    task();
    sem.release();
}