#pragma once
#include <functional>

// 定义函数指针
// typedef int(*handleFunc)(void* arg);
// using handleFunc = int(*)(void*);

// 1.定义文件描述符的读写事件 （位掩码形式）
enum class FDEvent
{
    TimeOut = 0x01,   // 超时事件（二进制 0001）
    ReadEvent = 0x02, // 读事件（二进制 0010)
    WriteEvent = 0x04 // 写事件（二进制 0100）
};

// 可调用对象包装器打包的是什么? 1. 函数指针 2. 可调用对象(可以向函数一样使用)
// 最终得到了地址, 但是没有调用
class Channel
{
public:
    using handleFunc = std::function<int(void*)>;
    // 构造函数：初始化fd、事件类型、三个回调函数、回调参数
    Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg);
    // 公有成员：三个回调函数（可直接赋值/调用） 
    handleFunc readCallback;  // 读事件回调
    handleFunc writeCallback; // 写事件回调
    handleFunc destroyCallback; // 销毁回调（释放连接资源）
    // 修改fd的写事件(检测 or 不检测)
    void writeEventEnable(bool flag); // 启用/禁用写事件
    // 判断是否需要检测文件描述符的写事件
    bool isWriteEventEnable();
    // 取出私有成员的值
    inline int getEvent()
    {
        return m_events;
    }
    inline int getSocket()
    {
        return m_fd;
    }
    inline const void* getArg()
    {
        return m_arg;
    }
private:
    // 文件描述符
    int m_fd;
    // 事件
    int m_events;
    // 回调函数的参数
    void* m_arg;
};

