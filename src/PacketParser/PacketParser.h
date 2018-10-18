//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_PACKETPARSER_H
#define NETWORKTIMINGTOOL_PACKETPARSER_H

#include <cstdlib>
#include <cstdint>

class PacketParser {
protected:
    void* packet = nullptr;
    void* payload = nullptr;
    size_t packet_size = 0;
    size_t payload_size = 0;

public:
    class Artefacts;
    enum Layer{
        ethernet,
        ipv4,
        ipv6,
        tcp,
        udp
    };
    virtual void parseHeader(void* packet, size_t size) = 0;
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size, Artefacts* artefacts);
    void* getPayload();
    size_t getPayloadSize();
};


#endif //NETWORKTIMINGTOOL_PACKETPARSER_H
