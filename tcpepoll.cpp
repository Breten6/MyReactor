// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>
// #include <sys/socket.h>
// #include <sys/types.h>          
// #include <arpa/inet.h>
// #include <sys/fcntl.h>
// #include <sys/epoll.h>
// #include <netinet/tcp.h>      // TCP_NODELAY
// #include "InetAddress.h"
// #include "Socket.h"
// #include "Epoll.h"
// #include "EventLoop.h"
// #include "TcpServer.h"
#include "EchoServer.h"
int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    // TcpServer tcpserver(argv[1],atoi(argv[2]));
    // tcpserver.start();
    EchoServer echoserver(argv[1],atoi(argv[2]));
    echoserver.Start();
  return 0;
}