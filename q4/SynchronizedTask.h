#ifndef SYNC_TASK
#define SYNC_TASK
#include "mbed.h"
class SynchronizedTask{
    public:
    void execute();
    SynchronizedTask(std::function<void()> task);
    private:
    std::function<void()> task;
};
#endif