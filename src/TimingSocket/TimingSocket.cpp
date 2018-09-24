#include "TimingSocket.h"
#include "../TimeSources/CPUTiming.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>

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

TimingSocket::TimingSocket(){
    best_timesource = TimeSources::best_timesource();
}

void TimingSocket::connect(std::string host, int port) {
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
    
    freeaddrinfo(res0);
}

void TimingSocket::write(const void *data, size_t size) {
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

uint64_t TimingSocket::writeAndTimeResponse(const void *data, size_t size) {
    uint8_t tmp_buf;
    uint64_t timing;
    write(data, size);
    /*start timing*/
    timing = best_timesource();
    ::recv(sock, &tmp_buf, 1, MSG_PEEK);
    /*end timing*/
    timing = best_timesource() - timing;
    return timing;
}

size_t TimingSocket::read(void *buf, size_t size) {
    if (state != SOCKSTATE_ESTABLISHED) {
        throw std::runtime_error(std::string("Socket is not ready for recieving"));
    }
    size_t size_recieved = ::recv(sock, buf, size, 0);
    if (size_recieved < 0) {
        throw std::runtime_error(std::string("Unable to recieve data. Reason: ")+std::string(strerror(errno)));
    }
    return size_recieved;
}

void TimingSocket::close() {
    ::close(sock);
    state = SOCKSTATE_CLOSED;
}
