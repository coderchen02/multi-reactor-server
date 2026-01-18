#pragma once
#include"EventLoop.h"
#include"Buffer.h"
#include"Channel.h"
#include"HttpRequest.h"
#include"HttpResponse.h"

//#define MSG_SEND_AUTO
struct TcpConnection
{
    struct EventLoop* evLoop;
    struct Channel* channel;
    struct Buffer* readBuf;
    struct Buffer* writeBuf;
    char name[32];
    //http 协议
    struct HttpResquest* request;
    struct HttpResponse* response;
};

//初始化
struct TcpConnection* TcpConnectionInit(int fd,struct EventLoop* evloop);
int tcpConnectionDestory(void* conn);