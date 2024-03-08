//
// Created by malte on 14.10.18.
//

#include "Ipv6Parser.h"
#include "../Tcp/TcpParser.h"
#include "../Udp/UdpParser.h"
#include "../Artefacts/Artefacts.h"

#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

bool Ipv6Parser::isExtensionHeader(uint8_t next_hdr) {
    return HeaderTypeMap.at((NextHeaderType)next_hdr)==hk_extension_header;
}

void Ipv6Parser::parseHeader(void* packet, size_t size) {
    if (size < MAIN_HEADER_SIZE) {
        throw std::invalid_argument("Ipv6 header must be at least 40 bytes");
    }
    this->packet = packet;
    this->packet_size = size;
    const struct ipv6_header * header = (struct ipv6_header*)packet;
    uint8_t version = (header->ver_trafcl_flow & 0xff)>>4;
    if (version != 0x6) {
        throw;
    }

    // start off with a payload size that is given in the packet.
    // This might be further reduced by extension headers
    payload_size = ntohs(header->payload_len);
    if (packet_size < 40 || (packet_size-40) < payload_size ) {
        /* payload size cannot be bigger than the packet itself */
        throw std::invalid_argument("payload size cannot be bigger than payload itself");
    }

    hop_limit = header->hop_limit;
    if(inet_ntop(AF_INET6, &header->addr_src, addr_src, INET6_ADDRSTRLEN) == nullptr) {
        throw std::invalid_argument("Packet has invalid ipv6 source address");
    }

    if(inet_ntop(AF_INET6, &header->addr_dst, addr_dst, INET6_ADDRSTRLEN) == nullptr) {
        throw std::invalid_argument("Packet has invalid ipv6 destination address");
    }

    next_hdr = header->next_hdr;
    const struct ipv6_ext_header* current_header = (ipv6_ext_header*)(header+1);
    while (isExtensionHeader(next_hdr)) {
        if (payload_size < 64) {
            /* extension headers are always multiples of 64 byte in size */
            throw;
        }
        next_hdr = current_header->next_hdr;
        size_t offset_to_next_header = (size_t)current_header->hdr_len*8+8;
        if (offset_to_next_header > payload_size) {
            /* there must be enough space left in the payload to contain the next header */
            throw;
        }
        payload_size -= offset_to_next_header;
        current_header = (ipv6_ext_header*)((uint8_t*)current_header+offset_to_next_header);
    }
    if (next_hdr == NextHeaderType::ipv6_nonxt) {
        /* There is no payload.*/
        payload = nullptr;
        payload_size = 0;
        return;
    }
    /* if the next header type is NOT an extension, it is probably an upper layer packet */
    payload = (void*)current_header;
}

void Ipv6Parser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size, Artefacts* artefacts) {
    Ipv6Parser current_parser;
    current_parser.parseHeader(packet, size);
    artefacts->ip_type = Layer::ipv6;
    strncpy(artefacts->src_ip, current_parser.getSrcAddress(), INET6_ADDRSTRLEN);
    strncpy(artefacts->dst_ip, current_parser.getDstAddress(), INET6_ADDRSTRLEN);
    if (layer == Layer::ipv6) {
        *payload = current_parser.getPayload();
        *payload_size = current_parser.getPayloadSize();
    }else{
        switch (current_parser.next_hdr) {
            case NextHeaderType::tcp:
                TcpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size, artefacts);
                break;
            case NextHeaderType::udp:
                UdpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size, artefacts);
                break;
            default:
                throw;
                break;
        }
    }
}

const char* Ipv6Parser::getSrcAddress() {
    return addr_src;
}

const char* Ipv6Parser::getDstAddress() {
    return addr_dst;
}
