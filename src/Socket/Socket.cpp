//
// Created by malte on 29.10.18.
//

#include "Socket.h"
#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <strings.h>
#include <cerrno>

int Socket::enableEpollWithEvents(uint32_t events, int sock) {
    struct epoll_event event;
    bzero(&event, sizeof(event));
    int epoll_fd = epoll_create1(0);

    if(epoll_fd == -1)
    {
        fprintf(stderr, "Failed to create epoll file descriptor. Reason: %s\n", strerror(errno));
        throw;
    }

    event.events = events;
    event.data.fd = sock;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event))
    {
        fprintf(stderr, "Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        throw;
    }
    return epoll_fd;
}

bool Socket::socketPeerClosed(int epfd) {
    struct epoll_event event;
    bzero(&event, sizeof(event));
    //event.events = EPOLLRDHUP;
    int res = epoll_wait(epfd, &event, 1, 1);
    if (res == -1) {
        throw;
    }
    if (res == 0) {
        // if no event is sent, the client is probably still there
        return false;
    }
    // only other case:
    // one event occured
    return (event.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) != 0;
}
