#pragma once
#include "Epoll.h"
#include <functional>
#include <memory>
#include <sys/syscall.h>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <map>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include "Connection.h"
class Channel;
class Epoll;
class Connection;
using spConnection=std::shared_ptr<Connection>;
class EventLoop
{
private:
    int  timetvl_;
    int  timeout_;  
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_; 
    pid_t threadid_;
    std::queue<std::function<void()>> taskqueue_;
    std::mutex mutex_;
    std::mutex mmutex_;
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;
    int timerfd_;
    std::unique_ptr<Channel> timerchannel_;
    bool mainloop_;  
    std::map<int,spConnection> conns_;
    std::function<void(int)>  timercallback_;
public:
    EventLoop(bool mainloop,int timetvl=30,int timeout=80);                 
    ~EventLoop();

    void run();
    void updatechannel(Channel *ch);  
    void removechannel(Channel *ch);
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
    bool isinloopthread(); 
    void queueinloop(std::function<void()> fn);
    void wakeup();
    void handlewakeup();
    void handletimer(); 
    void newconnection(spConnection conn); 
    void settimercallback(std::function<void(int)> fn);
};