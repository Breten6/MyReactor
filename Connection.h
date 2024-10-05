#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include <memory>
#include <atomic>
#include <sys/syscall.h>
#include "Timestamp.h"
class Connection;
class EventLoop;
class Channel;
using spConnection=std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* loop_; 
    std::unique_ptr<Socket> clientsock_;
    std::unique_ptr<Channel> clientchannel_;
    Buffer inputbuffer_;
    Buffer outputbuffer_;
    std::atomic_bool disconnect_; //workers threads wiil check this value to avoid client disconnecting in the middle of process
    std::function<void(spConnection)> closecallback_;
    std::function<void(spConnection)> errorcallback_;    
    std::function<void(spConnection,std::string&)> onmessagecallback_;  
    std::function<void(spConnection)> sendcompletecallback_;
    Timestamp lastatime_; // update every time recieve packets
public:
    Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock);
    ~Connection();
    int fd()const;
    std::string ip() const;
    uint16_t port() const;
    void onmessage();
    void closecallback();
    void errorcallback();
    void setclosecallback(std::function<void(spConnection)> fn);
    void seterrorcallback(std::function<void(spConnection)> fn);
    void setonmessagecallback(std::function<void(spConnection,std::string&)> fn); 
    void writecallback();
    void setsendcompletecallback(std::function<void(spConnection)> fn);
    //every thread can call this send
    void send(const char* data, size_t size);
    //IO thread can directly call this func, worker thread can send this to IO thread
    void sendinloop(std::shared_ptr<std::string> data);
    bool timeout(time_t now,int val);
};