#include "Channel.h"

Channel::Channel(const std::unique_ptr<EventLoop>& loop,int fd):loop_(loop),fd_(fd)     
{

}

Channel::~Channel()                         
{
}
int Channel::fd()                     
{
    return fd_;
}

void Channel::useet()
{
    events_=events_|EPOLLET;
}
void Channel::enablereading(){
    events_=events_|EPOLLIN;
    loop_->updatechannel(this);
}
void Channel::disablereading(){
    events_=events_&~EPOLLIN;
    loop_->updatechannel(this);
}
void Channel::enablewriting(){
    events_=events_|EPOLLOUT;
    loop_->updatechannel(this);
}
void Channel::disablewriting(){
    events_=events_&~EPOLLOUT;
    loop_->updatechannel(this);
}
void Channel::disableall(){
    events_=0;
    loop_->updatechannel(this);
}
void Channel::remove(){
    disableall();
    loop_->removechannel(this);
}
void Channel::setinepoll(bool inepoll)
{
    inepoll_=inepoll;
}

void Channel::setrevents(uint32_t ev)        
{
    revents_=ev;
}

bool Channel::inpoll()
{
    return inepoll_;
}

uint32_t Channel::events() 
{
    return events_;
}

uint32_t Channel::revents() 
{
    return revents_;
}
void Channel::handleevent(){
    if (revents_ & EPOLLRDHUP)                
    {
        // printf("client(eventfd=%d) disconnected.\n",fd_);
        // close(fd_);  
        printf("EPOLLRDHUP\n");  
        closecallback_() ;     
    }                               
    else if (revents_ & (EPOLLIN|EPOLLPRI))  
    {
        printf("EPOLLIN|EPOLLPRI\n");
        readcallback_();
    }
    else if (revents_ & EPOLLOUT)          
    {
        printf("EPOLLOUT\n");
        writecallback_();
    }
    else 
    {
        // printf("client(eventfd=%d) error.\n",fd_);
        // close(fd_);       
        errorcallback_();
    }
}
void Channel::setreadcallback(std::function<void()> fn){
    readcallback_ = fn;
}

 void Channel::setclosecallback(std::function<void()> fn)    
 {
    closecallback_=fn;
 }

 void Channel::seterrorcallback(std::function<void()> fn)    
 {
    errorcallback_=fn;
 }
 void Channel::setwritecallback(std::function<void()> fn){
    writecallback_=fn;
 }