//
// Created by malte on 14.10.18.
//

#include <stdexcept>
#include "TcpParser.h"

void TcpParser::parseHeader(void* packet, size_t size) {
    if (size < MIN_HEADER_LENGTH) {
        throw std::invalid_argument("TCP header must be at least 20 bytes");
    }
    this->packet = packet;
    this->packet_size = size;
    const struct tcp_header * header = (struct tcp_header*)packet;
    sport = ntohs(header->th_sport);
    dport = ntohs(header->th_dport);
    seq = ntohl(header->th_seq);
    ack = ntohl(header->th_ack);
    header_length = ((header->th_offx2 & 0xf0)>>4)*4;
    payload = (uint8_t*)packet + header_length;
    payload_size = packet_size - header_length;
}

void TcpParser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size) {
    if (layer != Layer::tcp) {
        throw;
    }
    TcpParser current_parser;
    current_parser.parseHeader(packet, size);
    *payload = current_parser.getPayload();
    *payload_size = current_parser.getPayloadSize();
}