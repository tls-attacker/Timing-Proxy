#include "TimingSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "CPUTimingSocket/CPUTimingSocket.h"
#include "KernelTimingSocket/KernelTimingSocket.h"
#include "PCAPTimingSocket/PCAPTimingSocket.h"

struct addrinfo* retrieveConnectionCandidates(const std::string& host, int port) {
    struct addrinfo hints, *res0;
    int error;
    std::string port_str = std::to_string(port);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    error = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res0);
    if (error != 0){
        throw std::runtime_error(std::string("Unable retrieve connection candidates for host \""+host+"\""));
    }
    return res0;
}

void Socket::TimingSocket::connect(std::string host, uint16_t port) {
    this->host = host;
    this->port = port;
    struct addrinfo *res, *res0;
    res0 = retrieveConnectionCandidates(host, port);
    sock = -1;
    for (res = res0; res; res = res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype,
                   res->ai_protocol);
        if (sock < 0) {
            continue;
        }
        
        if (::connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
            ::close(sock);
            sock = -1;
            continue;
        }
        /* socket opened */
        state = SOCKSTATE_ESTABLISHED;
        break;
    }
    if (sock < 0) {
        throw std::runtime_error(std::string("Unable to open socket for host \""+host+":"+std::to_string(port)+"\""));
    }

    /* enable epoll with EPOLLRDHUP event*/
    epfd = Socket::enableEpollWithEvents(EPOLLERR | EPOLLHUP | EPOLLRDHUP , sock);
    freeaddrinfo(res0);
}

void Socket::TimingSocket::write(const void *data, size_t size) {
    if (state != SOCKSTATE_ESTABLISHED) {
        throw std::runtime_error(std::string("Socket is not ready for sending"));
    }
    ssize_t return_val = ::send(sock, data, size, 0);
    if (return_val < 0) {
        throw std::runtime_error(std::string("Unable to send data. Reason: ")+std::string(strerror(errno)));
    }
    else if (return_val != size){
        write((uint8_t*)data+return_val, size-return_val);
    }
}

ssize_t Socket::TimingSocket::read(void *buf, size_t size, bool blocking) {
    int flags = 0;
    if (!blocking) {
        flags |= MSG_DONTWAIT;
    }
    if (state != SOCKSTATE_ESTABLISHED) {
        throw std::runtime_error(std::string("Socket is not ready for recieving"));
    }
    ssize_t size_recieved = ::recv(sock, buf, size, flags);
    if (size_recieved < 0 && blocking) {
        throw std::runtime_error(std::string("Unable to recieve data. Reason: ")+std::string(strerror(errno)));
    }
    return size_recieved;
}

void Socket::TimingSocket::close() {
    ::close(sock);
    state = SOCKSTATE_CLOSED;
}

bool Socket::TimingSocket::socketPeerClosed() {
    return Socket::socketPeerClosed(epfd);
}

std::unique_ptr<Socket::TimingSocket> Socket::TimingSocket::createTimingSocket(KindOfSocket kind){
    switch (kind) {
        case CPU:
            return std::make_unique<CPUTimingSocket>();
        case Kernel:
            return std::make_unique<KernelTimingSocket>();
        case PCAP:
            return std::make_unique<PCAPTimingSocket>();
        default:
            throw;
    }
}
