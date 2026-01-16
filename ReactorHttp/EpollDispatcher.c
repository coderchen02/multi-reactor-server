#include "Dispatcher.h"
#include<sys/epoll.h>

#define MAX 520
struct EpollData
{
    int epfd;
    struct epoll_event* events;
};
static void* epollInit();
static int epollAdd(struct Channel* channel,struct EventLoop* evLoop);
static int epollRemove(struct Channel* channel,struct EventLoop* evLoop);
static int epollModify(struct Channel* channel,struct EventLoop* evLoop);
static int epollDispatch(struct EventLoop* evLoop,int timeout);//单位： s
static int epollClear(struct EventLoop* evLoop);

struct Dispatcher EpollDispatcher={
    epollInit,
    epollAdd,
    epollRemove,
    epollModify,
    epollDispatch,
    epollClear
};
static void* epollInit()
{
    struct EpollData* data=(struct EpollData*)malloc (sizeof(struct EpollData));
    data->epfd=epoll_create(10);//参数已经被废弃 大于0 就行
    if(data->epfd==-1)
    {
        perror("epoll_create");
        exit(0);
    }
    data->events=(struct epoll_event*)calloc(MAX,sizeof(struct epoll_event));

    return data;
}
static int epollAdd(struct Channel* channel,struct EventLoop* evLoop)
{

}
static int epollRemove(struct Channel* channel,struct EventLoop* evLoop)
{

}
static int epollModify(struct Channel* channel,struct EventLoop* evLoop)
{

}
static int epollDispatch(struct EventLoop* evLoop,int timeout)//单位： s
{

}
static int epollClear(struct EventLoop* evLoop)
{

}