#include "ThreadPool.h"
#include <assert.h>
#include <stdlib.h>

ThreadPool::ThreadPool(EventLoop* mainLoop, int count)
{
    m_index = 0;            // 轮询索引初始化为0（用于分发连接）
    m_isStart = false;
    m_mainLoop = mainLoop; // 绑定主Reactor（MainLoop）
    m_threadNum = count;   // 从Reactor线程数量
    m_workerThreads.clear();  // 清空存储WorkerThread的vector
}

ThreadPool::~ThreadPool()
{
    for (auto item : m_workerThreads)
    {
        delete item;
    }
}

void ThreadPool::run()
{
    assert(!m_isStart);
    // 当前线程必须是主Reactor线程（避免从线程启动线程池）
    if (m_mainLoop->getThreadID() != this_thread::get_id())
    {
        exit(0);
    }
    m_isStart = true;
    // 若从线程数>0，创建并启动所有从Reactor线程
    if (m_threadNum > 0)
    {
        for (int i = 0; i < m_threadNum; ++i)
        {
            WorkerThread* subThread = new WorkerThread(i);// 创建从线程
            subThread->run();     // 启动从线程（内部运行EventLoop::run()）
            m_workerThreads.push_back(subThread);  // 添加到容器管理
        }
    }
}
// 轮询获取空闲从 Reactor
EventLoop* ThreadPool::takeWorkerEventLoop()
{
    assert(m_isStart);
    //当前线程必须是主Reactor线程（避免从线程调用此接口）
    if (m_mainLoop->getThreadID() != this_thread::get_id())
    {
        exit(0);
    }
    // 从线程池中找一个子线程, 然后取出里边的反应堆实例
    EventLoop* evLoop = m_mainLoop;
    if (m_threadNum > 0)
    {
        evLoop = m_workerThreads[m_index]->getEventLoop();// 取当前索引的从Reactor
        m_index = ++m_index % m_threadNum;          // 索引自增并取模，实现循环轮询
    }
    return evLoop;
}
