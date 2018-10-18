//
// Created by malte on 14.10.18.
//

#include <stdexcept>
#include "Ipv4Parser.h"
#include "../Tcp/TcpParser.h"
#include "../Udp/UdpParser.h"
#include "../Artefacts/Artefacts.h"
#include <cstring>

void Ipv4Parser::parseHeader(void *packet, size_t size) {
    if (size < MIN_HEADER_LEN) {
        throw std::invalid_argument("Ipv4 packet must be at least 20 bytes");
    }
    this->packet = packet;
    this->packet_size = size;
    const struct ipv4_header * header = (struct ipv4_header*)packet;
    // check version
    u_int8_t version = (((header)->ip_vhl) >> 4);
    if (version != 0x4) {
        throw;
    }

    header_length = (((header)->ip_vhl) & 0x0f)*4;
    payload = (u_int8_t*)packet + header_length;
    payload_size = ntohs(header->ip_len) - header_length;
    proto = (IpProto)header->ip_p;
    if (inet_ntop(AF_INET, &header->ip_src, addr_src, INET_ADDRSTRLEN) == nullptr) {
        throw std::invalid_argument("Packet has invalid ipv4 source address");
    }

    if(inet_ntop(AF_INET, &header->ip_dst, addr_dst, INET_ADDRSTRLEN) == nullptr){
        throw std::invalid_argument("Packet has invalid ipv4 destination address");
    }
}

void Ipv4Parser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size, Artefacts* artefacts) {
    Ipv4Parser current_parser;
    current_parser.parseHeader(packet, size);
    artefacts->ip_type = Layer::ipv4;
    strncpy(artefacts->src_ip, current_parser.getSrcAddress(), INET_ADDRSTRLEN);
    strncpy(artefacts->dst_ip, current_parser.getDstAddress(), INET_ADDRSTRLEN);
    if (layer == Layer::ipv4) {
        *payload = current_parser.getPayload();
        *payload_size = current_parser.getPayloadSize();
    }else{
        switch (current_parser.proto) {
            case IpProto::tcp:
                TcpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size, artefacts);
                break;
            case IpProto::udp:
                UdpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size, artefacts);
                break;
            default:
                throw;
                break;
        }
    }
}

const char* Ipv4Parser::getSrcAddress() {
    return addr_src;
}

const char* Ipv4Parser::getDstAddress() {
    return addr_dst;
}