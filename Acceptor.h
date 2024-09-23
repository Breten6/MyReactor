#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor
{
private:
    EventLoop *loop_; 
    Socket *servsock_;
    Channel *acceptchannel_; 
    std::function<void(Socket*)> newconnectioncb_;
public:
    Acceptor(EventLoop *loop,const std::string &ip,const uint16_t port);
    ~Acceptor();
    void newconnection();
    void setnewconnectioncb( std::function<void(Socket*)> fn);
};