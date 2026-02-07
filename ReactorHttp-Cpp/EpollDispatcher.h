#pragma once
#include "Channel.h"
#include "EventLoop.h"
#include "Dispatcher.h"
#include <string>
#include <sys/epoll.h>
using namespace std;

class EpollDispatcher : public Dispatcher
{
public:
    EpollDispatcher(EventLoop* evloop);
    ~EpollDispatcher();
    // 添加
    int add() override;
    // 删除
    int remove() override;
    // 修改
    int modify() override;
    // 事件监测
    int dispatch(int timeout = 2) override; // 单位: s

private:
    int epollCtl(int op);  私有函数：封装 epoll_ctl 调用，统一处理 ADD/DEL/MOD 逻辑

private:
    int m_epfd;
    struct epoll_event* m_events;  //用于接收 epoll_wait 返回的触发事件，大小 m_maxNode 可根据业务调整
    const int m_maxNode = 520;
};