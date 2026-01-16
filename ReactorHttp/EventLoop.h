#pragma once 
#include"Dispatcher.h"

extern struct Dispatcher EpollDispatcher;
struct EventLoop
{
    struct Dispatcher* Dispatcher;
    void* dispatcherData;
};