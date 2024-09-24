#include "Epoll.h"

Epoll::Epoll(){
    if ((epollfd_=epoll_create(1))==-1)
    {
        printf("epoll_create() failed(%d).\n",errno); exit(-1);
    }      
}
Epoll::~Epoll(){
    close(epollfd_);
}

void Epoll::updatechannel(Channel *ch){
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();
    if(ch->inpoll()){
        if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&ev)==-1){
            printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
        }
    }else{
        if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&ev)==-1){
            printf("epoll_ctl() failed(%d).\n",errno); exit(-1);
        }
        ch->setinepoll(true);
    }
}
void Epoll::removechannel(Channel *ch){
    if(ch->inpoll()){
        printf("remove channel\n");
        if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,ch->fd(),0)==-1){
            printf("epoll_ctl() removed(%d).\n",errno); exit(-1);
        }
    }
}
std::vector<Channel*> Epoll::loop(int timeout){
    std::vector<Channel*> channels; 

    bzero(events_,sizeof(events_));
    int infds=epoll_wait(epollfd_,events_,10,timeout);
    
    if (infds < 0)
    {
        perror("epoll_wait() failed"); exit(-1);
    }

    if (infds == 0)
    {
        //printf("epoll_wait() timeout.\n");
        return channels;
    }
    for (int ii=0;ii<infds;ii++) {
        Channel* ch = (Channel*)events_[ii].data.ptr;
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }  
    return channels;
}