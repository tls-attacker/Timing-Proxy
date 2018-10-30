//
// Created by malte on 29.10.18.
//

#ifndef NETWORKTIMINGTOOL_SOCKET_H
#define NETWORKTIMINGTOOL_SOCKET_H


#include <cstdint>

namespace Socket{
    static int enableEpollWithEvents(uint32_t events, int sock);
};


#endif //NETWORKTIMINGTOOL_SOCKET_H
