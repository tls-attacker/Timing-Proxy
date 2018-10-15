//
// Created by malte on 14.10.18.
//

#include "Ipv4Parser.h"
#include "../Tcp/TcpParser.h"
#include "../Udp/UdpParser.h"

void Ipv4Parser::parseHeader(void *packet, size_t size) {
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
}

void Ipv4Parser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size) {
    Ipv4Parser current_parser;
    current_parser.parseHeader(packet, size);
    if (layer == Layer::ipv4) {
        *payload = current_parser.getPayload();
        *payload_size = current_parser.getPayloadSize();
    }else{
        switch (current_parser.proto) {
            case IpProto::tcp:
                TcpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size);
                break;
            case IpProto::udp:
                UdpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size);
                break;
            default:
                throw;
                break;
        }
    }
}