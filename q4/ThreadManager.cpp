#include "ThreadManager.h"
#include "vv_time.h"
#include <cstdio>
#include <vector>

ThreadManager::ThreadManager() {
    threadCount = 0;
}

void ThreadManager::StartThread(SynchronizedTask* task) {
    mutex.lock();
    if (threadCount == MAX_THREAD_COUNT || TIME_GetNow() == lastTime) {
        mutex.unlock();
        return;
    }
    lastTime = TIME_GetNow();
    threadCount++;
    mutex.unlock();
    printf("creating thread\n\n");
    Thread* thread = new Thread();
    thread->start([=] {
        task->execute();
        mutex.lock();
        threadCount--;
        mutex.unlock();
        });
}