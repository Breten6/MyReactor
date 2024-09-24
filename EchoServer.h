#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_; //work threads


public:
    EchoServer(const std::string &ip,const uint16_t port, int subthreadnum = 3, int workthreadnums=5);
    ~EchoServer();

    void Start();

    void HandleNewConnection(spConnection conn);     // handle new connection
    void HandleClose(spConnection conn);                      // close client connection call back in TCPserver 
    void HandleError(spConnection conn);                       
    void HandleMessage(spConnection conn,std::string& message);    
    void HandleSendComplete(spConnection conn);
    void OnMessage(spConnection conn,std::string& message);
    // void HandleTimeOut(EventLoop *loop);
};