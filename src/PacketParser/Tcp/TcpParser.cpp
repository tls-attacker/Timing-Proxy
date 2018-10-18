//
// Created by malte on 14.10.18.
//

#include <stdexcept>
#include "TcpParser.h"
#include "../Artefacts/Artefacts.h"
#include <arpa/inet.h>

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

void TcpParser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size, Artefacts* artefacts) {
    if (layer != Layer::tcp) {
        throw;
    }
    TcpParser current_parser;
    current_parser.parseHeader(packet, size);
    artefacts->payload_type = Layer::tcp;
    artefacts->src_port = current_parser.getSrcPort();
    artefacts->dst_port = current_parser.getDstPort();
    *payload = current_parser.getPayload();
    *payload_size = current_parser.getPayloadSize();
}

uint16_t TcpParser::getSrcPort() {
    return sport;
}

uint16_t TcpParser::getDstPort() {
    return dport;
}