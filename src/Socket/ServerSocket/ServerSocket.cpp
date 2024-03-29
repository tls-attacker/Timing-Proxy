#include "ServerSocket.h"
#include <errno.h>
#include <stdexcept>
#include <unistd.h>
#include <strings.h>
#include <cstring>
#include <sys/epoll.h>
#include <iostream>

Socket::ServerSocket::~ServerSocket() {
    if (has_client) {
        close_client();
    }
    close();
}

void Socket::ServerSocket::bind(int port) {
    this->port = port;
    struct sockaddr_in6 serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //serv_addr.sin6_len = sizeof(serv_addr);
    serv_addr.sin6_family = AF_INET6;
    //serv_addr.sin6_addr = INADDR_ANY;
    serv_addr.sin6_port = htons(port);
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error(std::string("ServerSocket: Unable to open socket. Reason: ")+std::string(strerror(errno)));
    }
    int option = 0;
    if(setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &option, sizeof(option))){
        throw std::runtime_error(std::string("ServerSocket: Can't listen on ipv4 and ipv6. Reason: ")+std::string(strerror(errno)));
    }

    int enable=1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable))){
        throw std::runtime_error(std::string("ServerSocket: Can't set SO_REUSEADDR. Reason: ")+std::string(strerror(errno)));
    }

    if (::bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        throw std::runtime_error(std::string("ServerSocket: Unable to bind socket. Reason: ")+std::string(strerror(errno)));
    }
}

void Socket::ServerSocket::listen() {
    int ret = ::listen(sock, 2);
    if (ret != 0) {
        throw std::runtime_error(std::string("ServerSocket: Unable to listen on socket. Reason: ")+std::string(strerror(errno)));
    }
}

void Socket::ServerSocket::accept() {
   client_sock = ::accept(sock, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_sock < 0) {
       throw std::runtime_error(std::string("ServerSocket: Unable to accept client socket. Reason: ")+std::string(strerror(errno)));
    }
    /* enable epoll with EPOLLRDHUP event*/
    epfd = Socket::enableEpollWithEvents(EPOLLERR | EPOLLHUP | EPOLLRDHUP, sock);
    has_client = true;
}

void Socket::ServerSocket::close_client() {
    ::close(client_sock);
    ::close(epfd);
    has_client = false;
}

void Socket::ServerSocket::close() {
    ::close(sock);
}

ssize_t Socket::ServerSocket::read(void* buf, size_t size, bool blocking) {
    int flags = 0;
    if (!blocking) {
        flags |= MSG_DONTWAIT;
    }
    ssize_t size_recieved = ::recv(client_sock, buf, size, flags);
    if (size_recieved < 0 && blocking) {
        throw std::runtime_error(std::string("Unable to recieve data. Reason: ")+std::string(strerror(errno)));
    }
    return size_recieved;
}

ssize_t Socket::ServerSocket::write(const void *data, size_t size) {
    ssize_t return_val = ::send(client_sock, data, size, 0);
    if (return_val < 0) {
        throw std::runtime_error(std::string("Unable to send data. Reason: ")+std::string(strerror(errno)));
    }
    else if (return_val != size){
        write((uint8_t*)data+return_val, size-return_val);
    }
    return return_val;
}

bool Socket::ServerSocket::socketPeerClosed() {
    return Socket::socketPeerClosed(epfd);
}
