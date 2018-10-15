//
// Created by malte on 14.10.18.
//

#include "EthernetParser.h"
#include "../Ipv4/Ipv4Parser.h"
#include "../Ipv6/Ipv6Parser.h"

void EthernetParser::parseHeader(void* packet, size_t size) {
    this->packet = packet;
    this->packet_size = size;
    const ethernet_header* header = (ethernet_header*)packet;
    dhost_addr = header->ether_dhost;
    shost_addr = header->ether_shost;
    payload = (u_char*)packet + EthernetParser::HEADER_SIZE;
    payload_size = packet_size - EthernetParser::HEADER_SIZE;
    type = (EtherType)ntohs(header->ether_type);
}

void EthernetParser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size) {
    EthernetParser current_parser;
    current_parser.parseHeader(packet, size);
    if (layer == Layer::ethernet) {
        *payload = current_parser.getPayload();
        *payload_size = current_parser.getPayloadSize();
    }else{
        switch (current_parser.type) {
            case EtherType::ipv4:
                Ipv4Parser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size);
                break;
            case EtherType::ipv6:
                Ipv6Parser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size);
                break;
            default:
                throw;
                break;
        }
    }
}