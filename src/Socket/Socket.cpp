//
// Created by malte on 29.10.18.
//

#include "Socket.h"
#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <strings.h>

int Socket::enableEpollWithEvents(uint32_t events, int sock) {
    struct epoll_event event;
    bzero(&event, sizeof(event));
    int epoll_fd = epoll_create1(sock);

    if(epoll_fd == -1)
    {
        fprintf(stderr, "Failed to create epoll file descriptor\n");
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
