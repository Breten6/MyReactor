#pragma once
#include "Epoll.h"
class Channel;
class Epoll;
class EventLoop
{
private:
    Epoll *ep_;
public:
    EventLoop();                 
    ~EventLoop();

    void run();
    Epoll *ep();
    void updatechannel(Channel *ch);   
};