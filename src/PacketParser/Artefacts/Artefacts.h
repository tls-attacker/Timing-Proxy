//
// Created by malte on 18.10.18.
//

#ifndef NETWORKTIMINGTOOL_ARTEFACTS_H
#define NETWORKTIMINGTOOL_ARTEFACTS_H

#include "../PacketParser.h"
#include <string>
#include <arpa/inet.h>

struct PacketParser::Artefacts {
    Layer ip_type = ipv4; /* ipv4 or ipv6 */
    char src_ip[INET6_ADDRSTRLEN];
    char dst_ip[INET6_ADDRSTRLEN];
    Layer payload_type = tcp; /* tcp or udp */
    uint16_t src_port = 0;
    uint16_t dst_port = 0;
};


#endif //NETWORKTIMINGTOOL_ARTEFACTS_H
