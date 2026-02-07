#include "Dispatcher.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "EpollDispatcher.h"

EpollDispatcher::EpollDispatcher(EventLoop* evloop) : Dispatcher(evloop)
{
    m_epfd = epoll_create(10);
    if (m_epfd == -1)
    {
        perror("epoll_create");
        exit(0);
    }
    m_events = new struct epoll_event[m_maxNode];
    m_name = "Epoll";
}

EpollDispatcher::~EpollDispatcher()
{
    close(m_epfd);      // 关闭 epoll 实例描述符
    delete[]m_events;   // 释放事件数组内存
}

int EpollDispatcher::add()
{
    // 调用 epollCtl，传入 ADD 操作
    int ret = epollCtl(EPOLL_CTL_ADD);
    if (ret == -1)
    {
        perror("epoll_ctl add");
        exit(0);
    }
    return ret;
}

int EpollDispatcher::remove()
{
    // 调用 epollCtl，传入 DEL 操作
    int ret = epollCtl(EPOLL_CTL_DEL);
    if (ret == -1)
    {
        perror("epoll_ctl delete");
        exit(0);
    }
    // 通过 channel 释放对应的 TcpConnection 资源
    m_channel->destroyCallback(const_cast<void*>(m_channel->getArg()));

    return ret;
}

int EpollDispatcher::modify()
{
    // 调用 epollCtl，传入 MOD 操作
    int ret = epollCtl(EPOLL_CTL_MOD);
    if (ret == -1)
    {
        perror("epoll_ctl modify");
        exit(0);
    }
    return ret;
}

int EpollDispatcher::dispatch(int timeout)
{
    // 阻塞等待事件：参数依次是 epoll 实例、事件数组、数组大小、超时时间（毫秒）
    int count = epoll_wait(m_epfd, m_events, m_maxNode, timeout * 1000);
    // 遍历触发的事件
    for (int i = 0; i < count; ++i)
    {
        int events = m_events[i].events;// 触发的事件类型
        int fd = m_events[i].data.fd;   // 触发事件的 fd
        // 处理异常/挂断事件（对方关闭连接）
        if (events & EPOLLERR || events & EPOLLHUP)
        {
            // 对方断开了连接, 删除 fd
            // epollRemove(Channel, evLoop);
            continue;
        }
        // 处理读事件
        if (events & EPOLLIN)
        {
            // 通知 EventLoop 处理读事件
            m_evLoop->eventActive(fd, (int)FDEvent::ReadEvent);
        }
        // 处理写事件
        if (events & EPOLLOUT)
        {
            // 通知 EventLoop 处理写事件
            m_evLoop->eventActive(fd, (int)FDEvent::WriteEvent);
        }
    }
    return 0;
}

int EpollDispatcher::epollCtl(int op)
{
    struct epoll_event ev;
    // 绑定要操作的 fd（从 Channel 中获取）
    ev.data.fd = m_channel->getSocket(); 
    int events = 0;
    // 转换 Channel 的事件类型到 epoll 事件类型  
    //将自定义的 FDEvent::ReadEvent/WriteEvent 转换为 epoll 标准事件
    if (m_channel->getEvent() & (int)FDEvent::ReadEvent)
    {
        events |= EPOLLIN;// 读事件对应 EPOLLIN
    }
    if (m_channel->getEvent() & (int)FDEvent::WriteEvent)
    {
        events |= EPOLLOUT; // 写事件对应 EPOLLOUT
    }
    ev.events = events;  // 设置要监听的事件
    // 调用 epoll_ctl：参数依次是 epoll 实例、操作类型、目标 fd、事件结构体
    int ret = epoll_ctl(m_epfd, op, m_channel->getSocket(), &ev);
    return ret;
}
