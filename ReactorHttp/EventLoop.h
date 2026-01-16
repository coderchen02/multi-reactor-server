#pragma once 
#include"Dispatcher.h"

extern struct Dispatcher EpollDispatcher;
struct EventLoop
{
    Dispatcher* Dispatcher;
    void* dispatcherData;
};