#include "EchoServer.h"

// class EchoServer
// {
// private:
//     TcpServer tcpserver_;

// public:
//     EchoServer(const std::string &ip,const uint16_t port);
//     ~EchoServer();

//     void Start();

//     void HandleNewConnection(spConnection conn);     // handle new connection
//     void HandleClose(spConnection conn);                      // close client connection call back in TCPserver 
//     void HandleError(spConnection conn);                       
//     void HandleMessage(spConnection conn,std::string message);    
//     void HandleSendComplete(spConnection conn);
//     // void HandleTimeOut(EventLoop *loop);
// };

EchoServer::EchoServer(const std::string &ip,const uint16_t port, int subthreadnum,int workthreadnum):tcpserver_(ip,port,subthreadnum),threadpool_(workthreadnum, "WORKER")
{
    // not neccessary. only need to set the callback functions that we need
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}
EchoServer::~EchoServer()
{

}

void EchoServer::Start()                
{
    tcpserver_.start();
}

void EchoServer::HandleNewConnection(spConnection conn)    
{
    std::cout << "New Connection Come in." << std::endl;
    //printf("EchoServer::HandleNewConnection() thread is %d.\n",syscall(SYS_gettid));
    // more codes depends on what you need
}
 
void EchoServer::HandleClose(spConnection conn)  
{
    std::cout << "EchoServer conn closed." << std::endl;

    // more codes depends on what you need
}

void EchoServer::HandleError(spConnection conn)  
{
    std::cout << "EchoServer conn error." << std::endl;

    // more codes depends on what you need
}

void EchoServer::HandleMessage(spConnection conn,std::string& message)     
{
    //printf("EchoServer::HandleMessage() thread is %d.\n",syscall(SYS_gettid));
    // message="reply:"+message;
          
    // // int len=message.size();
    // // std::string tmpbuf((char*)&len,4);
    // // tmpbuf.append(message);
          
    // conn->send(message.data(),message.size());
    threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));
    
}
void EchoServer::OnMessage(spConnection conn,std::string& message){
    message="reply:"+message;
          
    // int len=message.size();
    // std::string tmpbuf((char*)&len,4);
    // tmpbuf.append(message);
          
    conn->send(message.data(),message.size());
}
void EchoServer::HandleSendComplete(spConnection conn)     
{
    std::cout << "Message send complete." << std::endl;

    // more codes depends on what you need
}


// void EchoServer::HandleTimeOut(EventLoop *loop)         
// {
//     std::cout << "EchoServer timeout." << std::endl;

//     // more codes depends on what you need
// }
