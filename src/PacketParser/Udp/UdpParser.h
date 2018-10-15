//
// Created by malte on 15.10.18.
//

#ifndef NETWORKTIMINGTOOL_UDPPARSER_H
#define NETWORKTIMINGTOOL_UDPPARSER_H

#include "../PacketParser.h"

class UdpParser : public PacketParser{
    const static size_t HEADER_LEN = 8;
    struct udp_header {
        uint16_t uh_sport; // source port
        uint16_t uh_dport; // destination port
        uint16_t uh_len; // length of udp packet
        uint16_t uh_sum; // checksum
    };
    uint16_t sport = 0;
    uint16_t dport = 0;
    uint16_t checksum = 0;
    size_t header_length = 0;
public:
    void parseHeader(void* packet, size_t size) override;
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size);
};


#endif //NETWORKTIMINGTOOL_UDPPARSER_H
