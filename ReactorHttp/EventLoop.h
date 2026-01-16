#pragma once 
#include"Dispatcher.h"

struct EventLoop
{
    Dispatcher* Dispatcher;
    void* dispatcherData;
};