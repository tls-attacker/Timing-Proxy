//
// Created by malte on 29.10.18.
//

#ifndef NETWORKTIMINGTOOL_SOCKET_H
#define NETWORKTIMINGTOOL_SOCKET_H


#include <cstdint>

namespace Socket{
    int enableEpollWithEvents(uint32_t events, int sock);
    bool socketPeerClosed(int epfd);
};


#endif //NETWORKTIMINGTOOL_SOCKET_H
