#include "EventLoop.h"
#include<assert.h>
#include<sys/socket.h>
struct EventLoop *eventLoopInit()
{
    return eventLoopInitEx(NULL);
}
//写数据
void taskWakeup(struct EventLoop* evLoop)
{
    const char* msg="我是要成为海贼王的男人！！！";
    write(evLoop->socketPair[0],msg,strlen(msg));
}
//读数据
int readLocalMessage(void* arg)
{
    struct EventLoop* evLoop=(struct EventLoop*)arg;
    char buf[256];
    read(evLoop->socketPair[1],buf,sizeof(buf));
    return 0;
}
struct EventLoop *eventLoopInitEx(const char *threadName)
{
    struct EventLoop* evLoop=(struct EventLoop*)malloc(sizeof(struct EventLoop));
    evLoop->isQuit=false;
    evLoop->threadID=pthread_self();
    pthread_mutex_init(&evLoop->mutex,NULL);
    //如果是主线程就指定线程名为MainThread 否则就是传入的参数threadName
    strcpy(evLoop->threadName,threadName==NULL ? "MainThread" : threadName);
    evLoop->dispatcher=&EpollDispatcher;
    evLoop->dispatcherData=evLoop->dispatcher->init();
    evLoop->head=evLoop->tail=NULL;
    //MAP
    evLoop->channelMap=channelMapInit(128);
    int ret=socketpair(AF_UNIX,SOCK_STREAM,0,evLoop->socketPair);
    if(ret==-1)
    {
        perror("socketpair");
        exit(0);
    }
    //指定规则：evloop->socketPair[0] 发送数据 evloop->socketPair[1] 接收数据
    struct Channel* channel=channelInit(evLoop->socketPair[1],ReadEvent,readLocalMessage,NULL,evLoop);
    //channel 添加到任务队列
    eventLoopAddTask(evLoop,channel,ADD);
    return evLoop;
}

int eventLoopRun(struct EventLoop *evLoop)
{
    assert(evLoop!=NULL);
    //取出事件分发和检测模型
    struct Dispatcher* dispatcher=evLoop->dispatcher;
    //比较线程ID是否正常
    if(evLoop->threadID!=pthread_self())
    {
        return -1;
    }
    //循环进行事件处理
    while(!evLoop->isQuit)
    {
        dispatcher->dispatch(evLoop,2);//超时时长 2s
    }
    return 0;
}

int eventActivate(struct EventLoop *evLoop, int fd, int event)
{
    if(fd<0||evLoop==NULL)
    {
        return -1;
    }
    //取出channel
    struct Channel* channel=evLoop->channelMap->list[fd];
    assert(channel->fd==fd);
    if(event & ReadEvent &&channel->readCallback)
    {
        channel->readCallback(channel->arg);
    }
    if(event& WriteEvent && channel->writeCallback)
    {
        channel->writeCallback(channel->arg);
    }
    return 0;
}

int eventLoopAddTask(struct EventLoop *evLoop, struct Channel *channel, int type)
{
    //加锁 保护共享资源
    pthread_mutex_lock(&evLoop->mutex);
    //创建新节点
    struct ChannelElement* node=(struct ChannelElement*)malloc(sizeof(struct ChannelElement));
    node->channel=channel;
    node->type=type;
    node->next=NULL;
    //链表为空
    if(evLoop->head==NULL)
    {
        evLoop->head=evLoop->tail=node;
    }
    else
    {
        evLoop->tail->next=node;
        evLoop->tail=node;
    }
    pthread_mutex_unlock(&evLoop->mutex);
    //处理节点
    /*
        1.对于链表节点的添加：可能是当前线程也可能是其他线程（主线程）
            1)修改fd的事件，当前子线程发起，当前子线程处理
            2)添加新的fd,添加任务节点的操作是由主线程发起的
        2.不能让主线程处理任务队列，需要由当前的子线程去处理
    */
   if(evLoop->threadID==pthread_self())
   {
    //当前是子线程
   }
   else
   {
    //当前是主线程  告诉子线程处理任务队列中的任务
    //1.子线程在工作 2.子线程被阻塞了：select,poll,epoll
    taskWakeup(evLoop);

   }
    return 0;
}
