#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"
int creatnonblocking();
class Socket{
    private:
        const int fd_;
        std::string ip_;
        uint16_t port_;
    public:
        Socket(int fd);
        ~Socket();
        std::string ip() const;
        uint16_t port() const;
        void setipport(const std::string ip,uint16_t port);

        int fd() const;
        void setreuseaddr(bool on);
        void setreuseport(bool on);
        void settcpnodelay(bool on);
        void setkeepalive(bool on);
        void bind(const InetAddress& servaddr);
        void listen(int size = 128);
        int accept(InetAddress & clientaddr);
};