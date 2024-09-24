#include "Connection.h"


Connection::Connection(const std::unique_ptr<EventLoop>& loop,std::unique_ptr<Socket> clientsock):
loop_(loop),clientsock_(std::move(clientsock)),disconnect_(false),clientchannel_(new Channel(loop_,clientsock_->fd()))
{
    // clientchannel_=new Channel(loop_,clientsock_->fd());   
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->useet();                 // edge trigger
    clientchannel_->enablereading(); 
}

Connection::~Connection()
{
    printf("Connection distructed。\n");
}

int Connection::fd()const{
    return clientsock_->fd();
}
std::string Connection::ip() const{
    return clientsock_->ip();
}
uint16_t Connection::port() const{
    return clientsock_->port();
}
void Connection::onmessage(){
    char buffer[1024];
    while (true)            
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));
        if (nread > 0)      // read data successfully
        {
            //printf("recv(eventfd=%d):%s\n",fd(),buffer);
            //send(fd(),buffer,strlen(buffer),0);
            inputbuffer_.append(buffer,nread);
        } 
        else if (nread == -1 && errno == EINTR)
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            //printf("recv(eventfd=%d):%s\n",fd(),inputbuffer_.data());
            while(true){
                int len;
                memcpy(&len, inputbuffer_.data(),4);
                if(inputbuffer_.size()<len+4) break;
                std::string message(inputbuffer_.data()+4,len);
                inputbuffer_.erase(0,len+4);
                printf("message (eventfd=%d):%s\n",fd(),message.c_str());
                // message="reply:"+message;
                
                // len=message.size();  
                // std::string tmpbuf((char*)&len,4);
                // tmpbuf.append(message);        
                // send(fd(),tmpbuf.data(),tmpbuf.size(),0);  
                onmessagecallback_(shared_from_this(), message);
            }

            break;
        } 
        else if (nread == 0)
        {  
            // printf("client(eventfd=%d) disconnected.\n",fd_);
            // close(fd_);   
            closecallback(); 
            break;
        }
    }
}
void Connection::closecallback(){
    // printf("client(eventfd=%d) disconnected.\n",fd());
    // close(fd());            // disconnect client
    disconnect_=true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}
void Connection::errorcallback(){
    // printf("client(eventfd=%d) error.\n",fd());
    // close(fd());  
    disconnect_=true;
    clientchannel_->remove();
    errorcallback_(shared_from_this());
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)    
{
    closecallback_=fn;
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)    
{
    errorcallback_=fn;
}
void Connection::setonmessagecallback(std::function<void(spConnection,std::string&)> fn)    
{
    onmessagecallback_=fn;
}
void Connection::setsendcompletecallback(std::function<void(spConnection)> fn){
    sendcompletecallback_=fn;
}
void Connection::send(const char* data, size_t size){
        if (disconnect_==true) {  printf("client disconnected, no more message sending\n"); return;}
        outputbuffer_.genmessage(data,size);
        // register write evenet
        clientchannel_->enablewriting();

}

void Connection::writecallback(){
    int written = ::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);//try send everything inside buffer
    if(written>0){
        outputbuffer_.erase(0,written);
    }
    if(outputbuffer_.size() == 0){ //avoid busy loop
        clientchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}