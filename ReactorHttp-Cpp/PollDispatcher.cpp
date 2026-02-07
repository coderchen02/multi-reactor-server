#include "Dispatcher.h"
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include "PollDispatcher.h"

PollDispatcher::PollDispatcher(EventLoop* evloop) : Dispatcher(evloop)
{
    m_maxfd = 0; // 初始化有效 fd 的最大下标
    // 分配 pollfd 数组，大小为 m_maxNode
    m_fds = new struct pollfd[m_maxNode];
    // 初始化数组：所有 fd 设为 -1（无效），事件设为 0
    for (int i = 0; i < m_maxNode; ++i)
    {
        m_fds[i].fd = -1;     // fd=-1 表示该位置未使用
        m_fds[i].events = 0;  // 监听的事件：初始无
        m_fds[i].revents = 0; // 触发的事件：初始无
    }
    m_name = "Poll";  // 设置分发器名称
}

PollDispatcher::~PollDispatcher()
{
    delete[]m_fds;
}

int PollDispatcher::add()
{
    // 1. 转换事件类型：自定义 FDEvent → poll 标准事件
    int events = 0;
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        events |= POLLIN;
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        events |= POLLOUT;
    }
    int i = 0;
     // 2. 遍历 pollfd 数组，找空闲位置（fd=-1）
    for (; i < m_maxNode; ++i)
    {
        if (m_fds[i].fd == -1)
        {
            // 填充空闲位置：设置监听事件和 fd
            m_fds[i].events = events;
            m_fds[i].fd = m_channel->getSocket();
            // 更新 m_maxfd：确保是当前最大下标
            m_maxfd = i > m_maxfd ? i : m_maxfd;
            break;
        }
    }
    // 3. 检查是否超出数组容量
    if (i >= m_maxNode)
    {
        return -1;
    }
    return 0;
}

int PollDispatcher::remove()
{
    // 1. 遍历数组，找到对应 fd 的位置
    int i = 0;
    for (; i < m_maxNode; ++i)
    {
        // 重置该位置：fd=-1，事件清空
        if (m_fds[i].fd == m_channel->getSocket())
        {
            m_fds[i].events = 0;
            m_fds[i].revents = 0;
            m_fds[i].fd = -1;
            break;
        }
    }
    // 通过 channel 释放对应的 TcpConnection 资源
    m_channel->destroyCallback(const_cast<void*>(m_channel->getArg()));
    // 3. 检查是否找到 fd
    if (i >= m_maxNode)
    {
        return -1;
    }
    return 0;
}

int PollDispatcher::modify()
{
    // 1. 转换事件类型
    int events = 0;
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        events |= POLLIN;
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        events |= POLLOUT;
    }
    int i = 0;
    // 2. 遍历数组，找到对应 fd 并更新事件
    for (; i < m_maxNode; ++i)
    {
        if (m_fds[i].fd == m_channel->getSocket())
        {
            m_fds[i].events = events; // 覆盖原有事件
            break;
        }
    }
    if (i >= m_maxNode)
    {
        return -1;
    }
    return 0;
}

int PollDispatcher::dispatch(int timeout)
{
    // 1. 调用 poll() 阻塞等待事件
    // 参数：pollfd 数组、数组有效长度（m_maxfd+1）、超时时间（毫秒）
    int count = poll(m_fds, m_maxfd + 1, timeout * 1000);
    if (count == -1)
    {
        perror("poll");
        exit(0);
    }
    // 2. 遍历有效范围，处理触发的事件
    for (int i = 0; i <= m_maxfd; ++i)
    {
        if (m_fds[i].fd == -1)
        {
            continue;
        }

        if (m_fds[i].revents & POLLIN)
        {
            m_evLoop->eventActive(m_fds[i].fd, (int)FDEvent::ReadEvent);
        }
        if (m_fds[i].revents & POLLOUT)
        {
            m_evLoop->eventActive(m_fds[i].fd, (int)FDEvent::WriteEvent);
        }
    }
    return 0;
}
