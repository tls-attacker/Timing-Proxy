//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_PACKETPARSER_H
#define NETWORKTIMINGTOOL_PACKETPARSER_H

#include <cstdlib>
#include <arpa/inet.h>

class PacketParser {
protected:
    void* package = nullptr;
    void* payload = nullptr;
    size_t packet_size = 0;
    size_t payload_size = 0;

public:
    enum Layer{
        ethernet,
        ipv4,
        ipv6,
        tcp
    };
    virtual void parseHeader(void* package, size_t size) = 0;
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size);
    void* getPayload();
    size_t getPayloadSize();
};


#endif //NETWORKTIMINGTOOL_PACKETPARSER_H
