//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_ETHERNETPARSER_H
#define NETWORKTIMINGTOOL_ETHERNETPARSER_H

#include "../PacketParser.h"

class PacketParser;

class EthernetParser : public PacketParser{
    static const size_t HEADER_SIZE = 14;
    static const size_t ADDR_LEN = 6;
    enum EtherType {
        ipv4 = 0x0800,
        ipv6 = 0x86dd,
        other = 0x0
    };
    struct ethernet_header {
        uint8_t ether_dhost[ADDR_LEN];
        uint8_t ether_shost[ADDR_LEN];
        uint16_t ether_type;
    };
    const uint8_t * dhost_addr = nullptr;
    const uint8_t * shost_addr = nullptr;
    EtherType type = EtherType::other;

public:
    void parseHeader(void* package, size_t size) override;
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size);
};


#endif //NETWORKTIMINGTOOL_ETHERNETPARSER_H
