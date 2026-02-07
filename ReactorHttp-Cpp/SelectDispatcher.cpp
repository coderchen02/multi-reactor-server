#include "Dispatcher.h"
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include "SelectDispatcher.h"

SelectDispatcher::SelectDispatcher(EventLoop* evloop) :Dispatcher(evloop)
{
    FD_ZERO(&m_readSet); // 清空读事件集合
    FD_ZERO(&m_writeSet);// 清空写事件集合
    m_name = "Select";   // 设置分发器名称  
}

SelectDispatcher::~SelectDispatcher()
{
}

int SelectDispatcher::add()
{
    // 限制：select 的 fd 不能超过 FD_SETSIZE
    if (m_channel->getSocket() >= m_maxSize)
    {
        return -1;
    }
    setFdSet();
    return 0;
}

int SelectDispatcher::remove()
{
    // 将 fd 从对应事件集合中移除
    clearFdSet();
    // 通过 channel 释放对应的 TcpConnection 资源
    m_channel->destroyCallback(const_cast<void*>(m_channel->getArg()));

    return 0;
}

int SelectDispatcher::modify()
{
    setFdSet();
    clearFdSet();
    return 0;
}

int SelectDispatcher::dispatch(int timeout)
{
    // 1. 初始化超时时间：秒级 timeout 转换为 timeval 结构体
    struct timeval val;
    val.tv_sec = timeout;
    val.tv_usec = 0;
    // 2. 复制 fd 集合：select 会修改传入的集合，因此需用临时变量
    fd_set rdtmp = m_readSet;
    fd_set wrtmp = m_writeSet;
    // 3. 调用 select() 阻塞等待事件
    // 参数：最大 fd+1、读集合、写集合、异常集合、超时时间
    int count = select(m_maxSize, &rdtmp, &wrtmp, NULL, &val);
    if (count == -1)
    {
        perror("select");
        exit(0);
    }
    // 4. 遍历所有 fd，检测触发的事件
    for (int i = 0; i < m_maxSize; ++i)
    {
        if (FD_ISSET(i, &rdtmp))
        {
            m_evLoop->eventActive(i, (int)FDEvent::ReadEvent);
        }

        if (FD_ISSET(i, &wrtmp))
        {
            m_evLoop->eventActive(i, (int)FDEvent::WriteEvent);
        }
    }
    return 0;
}
// 将 Channel 的 fd 加入对应事件集合
void SelectDispatcher::setFdSet()
{
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        FD_SET(m_channel->getSocket(), &m_readSet); // 加入读集合
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        FD_SET(m_channel->getSocket(), &m_writeSet);// 加入写集合
    }
}
// 将 Channel 的 fd 从对应事件集合中移除
void SelectDispatcher::clearFdSet()
{
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        FD_CLR(m_channel->getSocket(), &m_readSet);// 移除读集合
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        FD_CLR(m_channel->getSocket(), &m_writeSet);// 移除写集合
    }
}
