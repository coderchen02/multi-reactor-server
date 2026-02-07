#include "WorkerThread.h"
#include <stdio.h>

// 子线程的回调函数
void WorkerThread::running()
{
    m_mutex.lock();  // 加锁保护m_evLoop的初始化
    // 1. 在从线程中创建EventLoop（从Reactor）
    m_evLoop = new EventLoop(m_name);
    m_mutex.unlock();
    // 2. 通知主线程：EventLoop已初始化完成
    m_cond.notify_one();
     // 3. 启动从Reactor的事件循环（阻塞直到EventLoop退出）
    m_evLoop->run();
}

WorkerThread::WorkerThread(int index)
{
    m_evLoop = nullptr;
    m_thread = nullptr;
    m_threadID = thread::id();
    m_name =  "SubThread-" + to_string(index);
}

WorkerThread::~WorkerThread()
{
    if (m_thread != nullptr)
    {
        delete m_thread;
    }
}

void WorkerThread::run()
{
    // 1. 创建从线程，绑定running()作为线程入口函数
    m_thread = new thread(&WorkerThread::running, this);
    // 2. 同步等待：主线程阻塞，直到从线程初始化好EventLoop
    unique_lock<mutex> locker(m_mutex);
    while (m_evLoop == nullptr)
    {
        m_cond.wait(locker);
    }
}
