//
// Created by malte on 15.10.18.
//

#include "UdpParser.h"
void UdpParser::parseHeader(void* packet, size_t size) {
    if (size < HEADER_LEN) {
        throw;
    }
    this->packet = packet;
    this->packet_size = size;
    const struct udp_header * header = (struct udp_header*)packet;
    sport = ntohs(header->uh_sport);
    dport = ntohs(header->uh_dport);
    checksum = ntohs(header->uh_sum);
    if (ntohs(header->uh_len) != packet_size) {
        throw;
    }
    payload = (uint8_t*)packet + HEADER_LEN;
    payload_size = packet_size - HEADER_LEN;
}

void UdpParser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size) {
    if (layer != Layer::udp) {
        throw;
    }
    UdpParser current_parser;
    current_parser.parseHeader(packet, size);
    *payload = current_parser.getPayload();
    *payload_size = current_parser.getPayloadSize();
}