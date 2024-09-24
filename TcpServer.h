#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>
#include "ThreadPool.h"
#include <memory>

class TcpServer
{
private:
    std::unique_ptr<EventLoop> mainloop_;
    std::vector<std::unique_ptr<EventLoop>> subloops_;
    int threadnum_;
    ThreadPool threadpool_;

    Acceptor acceptor_;
    std::map <int,spConnection> conns_;
    std::function<void(spConnection)> newconnectioncb_;          // EchoServer::HandleNewConnection()。
    std::function<void(spConnection)> closeconnectioncb_;        // EchoServer::HandleClose()。
    std::function<void(spConnection)> errorconnectioncb_;         // EchoServer::HandleError()。
    std::function<void(spConnection,std::string &message)> onmessagecb_;        // EchoServer::HandleMessage()。
    std::function<void(spConnection)> sendcompletecb_;            // EchoServer::HandleSendComplete()。
    std::function<void(EventLoop*)>  timeoutcb_;                       // EchoServer::HandleTimeOut()。
public:
    TcpServer(const std::string &ip,const uint16_t port, int threadnum = 3);
    ~TcpServer();

    void start(); 

    void newconnection(std::unique_ptr<Socket>clientsock); 
    void closeconnection(spConnection conn);
    void errorconnection(spConnection conn);
    void onmessage(spConnection conn,std::string& message);
    void sendcomplete(spConnection conn);
    void epolltimeout(EventLoop* loop);

    void setnewconnectioncb(std::function<void(spConnection)> fn);
    void setcloseconnectioncb(std::function<void(spConnection)> fn);
    void seterrorconnectioncb(std::function<void(spConnection)> fn);
    void setonmessagecb(std::function<void(spConnection,std::string &message)> fn);
    void setsendcompletecb(std::function<void(spConnection)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
};