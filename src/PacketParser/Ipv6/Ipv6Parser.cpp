//
// Created by malte on 14.10.18.
//

#include "Ipv6Parser.h"
#include "../Tcp/TcpParser.h"

bool Ipv6Parser::isExtensionHeader(uint8_t next_hdr) {
    for (int i = 0; i < NUM_EXTENSION_HEADERS; ++i) {
        if (next_hdr == EXTENSION_HEADERS[i]) {
            return true;
        }
    }
    return false;
}

void Ipv6Parser::parseHeader(void* package, size_t size) {
    this->package = package;
    this->packet_size = size;
    const struct ipv6_header * header = (struct ipv6_header*)package;
    uint8_t version = (uint8_t)header->ver_trafcl_flow >> 27;
    if (version != 0x6) {
        throw;
    }

    // start off with a payload size that is given in the packet.
    // This might be further reduced by extension headers
    payload_size = ntohs(header->payload_len);
    if (packet_size < 40 || (packet_size-40) < payload_size ) {
        /* payload size cannot be bigger than the packet itself */
        throw;
    }

    hop_limit = header->hop_limit;
    addr_src = &header->addr_src;
    addr_dst = &header->addr_dst;

    next_hdr = header->next_hdr;
    const struct ipv6_ext_header* current_header = (ipv6_ext_header*)(header+1);
    while (isExtensionHeader(next_hdr)) {
        if (payload_size < 64) {
            /* extension headers are always multiples of 64 byte in size */
            throw;
        }
        next_hdr = header->next_hdr;
        size_t offset_to_next_header = (size_t)current_header->hdr_len*8+8;
        if (offset_to_next_header > payload_size) {
            /* there must be enough space left in the payload to contain the next header */
            throw;
        }
        payload_size -= offset_to_next_header;
        current_header = (ipv6_ext_header*)((uint8_t*)current_header+offset_to_next_header);
    }
    if (current_header->next_hdr == NextHeaderType::ipv6_nonxt) {
        /* There is no payload.*/
        payload = nullptr;
        payload_size = 0;
        return;
    }
    /* if the next header type is NOT an extension, it is probably an upper layer packet */
    payload = (void*)current_header;
}

void Ipv6Parser::decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size) {
    Ipv6Parser current_parser;
    current_parser.parseHeader(packet, size);
    if (layer == Layer::ipv6) {
        *payload = current_parser.getPayload();
        *payload_size = current_parser.getPayloadSize();
    }else{
        switch (current_parser.next_hdr) {
            case NextHeaderType::tcp:
                TcpParser::decodeUntil(layer, current_parser.getPayload(), current_parser.getPayloadSize(), payload, payload_size);
                break;
            default:
                throw;
                break;
        }
    }
}