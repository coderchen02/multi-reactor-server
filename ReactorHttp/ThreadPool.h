#pragma once
#include"EventLoop.h"
#include<stdio.h>
#include"WorkerThread.h"

struct ThreadPool
{
    //主线程的反应堆模型
    struct EventLoop* mainLoop;
    bool isStart;
    int threadNum;  //  线程池里面子线程的个数
    struct WorkerThread* workerThreads;
    int index;
};

//初始化线程池
struct ThreadPool* threadPoolInit(struct EventLoop* mainLoop,int count);
//启动线程池
void threadPoolRun(struct ThreadPool* pool);