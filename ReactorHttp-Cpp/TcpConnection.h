#pragma once
#include "EventLoop.h"
#include "Buffer.h"
#include "Channel.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

//#define MSG_SEND_AUTO

class TcpConnection
{
public:
    TcpConnection(int fd, EventLoop* evloop);
    ~TcpConnection();

    static int processRead(void* arg);
    static int processWrite(void* arg);
    static int destroy(void* arg);
private:
    string m_name;        // 连接名称（标识 fd）
    EventLoop* m_evLoop;  // 关联的事件循环
    Channel* m_channel;   // 连接对应的 Channel（fd+事件+回调）
    Buffer* m_readBuf;    // 读缓冲区（存储接收的 HTTP 请求）
    Buffer* m_writeBuf;   // 写缓冲区（存储待发送的 HTTP 响应）
    // http 协议
    HttpRequest* m_request;
    HttpResponse* m_response;
};