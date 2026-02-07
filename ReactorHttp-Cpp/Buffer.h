#pragma once
#include <string>
using namespace std;

class Buffer
{
public:
    Buffer(int size);
    ~Buffer();

    // 扩容
    void extendRoom(int size);
    // 得到剩余的可写的内存容量
    inline int writeableSize()
    {
        return m_capacity - m_writePos;
    }
    // 得到剩余的可读的内存容量
    inline int readableSize()
    {
        return m_writePos - m_readPos;
    }
    // 写内存 1. 直接写 2. 接收套接字数据
    int appendString(const char* data, int size);
    int appendString(const char* data);
    int appendString(const string data);
    int socketRead(int fd);
    // 根据\r\n取出一行, 找到其在数据块中的位置, 返回该位置
    char* findCRLF();
    // 发送数据
    int sendData(int socket);    // 指向内存的指针
    // 得到读数据的起始位置
    inline char* data()
    {
        return m_data + m_readPos;
    }
    inline int readPosIncrease(int count)
    {
        m_readPos += count;
        return m_readPos;
    }
private:
    char* m_data;   // 缓冲区内存起始地址
    int m_capacity; // 缓冲区总容量
    int m_readPos = 0;// 读指针：下一个要读取的位置（初始0）
    int m_writePos = 0;// 写指针：下一个要写入的位置（初始0）
};

