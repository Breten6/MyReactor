#include "EventLoop.h"
int createtimerfd_(int sec=30){
    int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);
    struct itimerspec timeout;
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;// set it to 5 for testing
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd,0,&timeout,0);
    return tfd;
}
EventLoop::EventLoop(bool mainloop,int timetvl,int timeout):
mainloop_(mainloop),ep_(new Epoll),timetvl_(timetvl),timeout_(timeout),wakeupfd_(eventfd(0,EFD_NONBLOCK)),
wakechannel_(new Channel(this,wakeupfd_)),timerfd_(createtimerfd_(timeout)),timerchannel_(new Channel(this,timerfd_))                
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakechannel_->enablereading();
    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer,this));
    timerchannel_->enablereading();
}

EventLoop::~EventLoop()
{

}
#include <unistd.h>
#include <syscall.h>
void EventLoop::run()                      
{
    threadid_ = syscall(SYS_gettid);
    printf("Eventloop::run() thread is %ld.\n",syscall(SYS_gettid));
    while (true)   
    {
       std::vector<Channel *> channels=ep_->loop(10*1000);    
        if(channels.size() == 0){
            epolltimeoutcallback_(this);
        }else{
            for (auto &ch:channels)
            {
                ch->handleevent();      
            }
        }

    }
}
void EventLoop::updatechannel(Channel *ch)                        
{
    ep_->updatechannel(ch);
}
void EventLoop::removechannel(Channel *ch){
    ep_->removechannel(ch);
}
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn){
    epolltimeoutcallback_=fn;
}
bool EventLoop::isinloopthread(){
    return threadid_ == syscall(SYS_gettid);
}
void EventLoop::queueinloop(std::function<void()> fn){
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }
    wakeup();
}
void EventLoop::wakeup(){
    uint64_t val = 1;
    write(wakeupfd_,&val,sizeof(val));
}
void EventLoop::handlewakeup(){
    printf("handlewakeup() thread id is %d\n",syscall(SYS_gettid));
    uint64_t val;
    read(wakeupfd_,&val,sizeof(val));
    std::function<void()> fn;
    std::lock_guard<std::mutex> gd(mutex_);
    while (!taskqueue_.empty()){
        fn = std::move(taskqueue_.front());
        taskqueue_.pop();
        fn();
    }
    {
        /* code */
    }
    
}
void EventLoop::handletimer()                                                 
{
    //reset alarm
    struct itimerspec timeout;
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timeout_;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_,0,&timeout,0);

    if (mainloop_){
        //printf("mainloop alarm ring\n");
    }
    else{
        //printf("subloops alarm ring\n");
        printf("EventLoop::handletimer() thread is %d. fd \n",syscall(SYS_gettid));
        time_t now=time(0);
        for (auto it=conns_.begin();it!=conns_.end();)
        {
            printf(" %d",it->first);
            if (it->second->timeout(now,timeout_)) 
            {
                printf("EventLoop::handletimer()1  thread is %d.\n",syscall(SYS_gettid)); 
                timercallback_(it->first);             // 从TcpServer的map中删除超时的conn。
                std::lock_guard<std::mutex> gd(mmutex_);
                it = conns_.erase(it);                // 从EventLoop的map中删除超时的conn。
            } else it++;
        }
        printf("\n");
    }

}
void EventLoop::newconnection(spConnection conn){
    {
        std::lock_guard<std::mutex> gd(mmutex_);
            conns_[conn->fd()]=conn;
    }

}
void EventLoop::settimercallback(std::function<void(int)> fn){
    timercallback_=fn;
}