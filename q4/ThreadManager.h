#ifndef THREAD_MANAGER
#define THREAD_MANAGER
#include "mbed.h"
#include "SynchronizedTask.h"

#define MAX_THREAD_COUNT 10

class ThreadManager{
    public:
    void StartThread(SynchronizedTask* task);
    ThreadManager();
    private:
    int threadCount;
    uint32_t lastTime;
    Mutex mutex;

};
#endif