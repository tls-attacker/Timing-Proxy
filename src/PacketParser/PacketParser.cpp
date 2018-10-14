//
// Created by malte on 14.10.18.
//

#include "PacketParser.h"
#include "Ethernet/EthernetParser.h"

void* PacketParser::getPayload() {
    return payload;
}

size_t PacketParser::getPayloadSize() {
    return payload_size;
}

void PacketParser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size) {
    EthernetParser::decodeUntil(layer, packet, size, payload, payload_size);
}