//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_IPV4PARSER_H
#define NETWORKTIMINGTOOL_IPV4PARSER_H

#include "../PacketParser.h"
#include <netinet/in.h>
#include <arpa/inet.h>

class Ipv4Parser : public PacketParser {
    const static size_t MIN_HEADER_LEN = 20;
    enum IpProto {
        tcp = 0x6,
        udp = 0x11,
        other
    };
    struct ipv4_header {
        u_int8_t ip_vhl; // version << 4 | header_length >> 2
        u_int8_t ip_tos; // type of service
        u_int16_t ip_len; // total length
        u_int16_t ip_id; // identification
        u_int16_t ip_off; // fragment offset field
        u_int8_t ip_ttl; // time to live
        u_int8_t ip_p; // protocol
        u_int16_t ip_sum; // checksum
        const struct in_addr ip_src,ip_dst; // source and dest address
    };
    size_t header_length = 20;
    IpProto proto = other;
    char addr_src[INET_ADDRSTRLEN];
    char addr_dst[INET_ADDRSTRLEN];
public:
    void parseHeader(void* packet, size_t size) override;
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size, Artefacts* artefacts);
    const char* getSrcAddress();
    const char* getDstAddress();
};


#endif //NETWORKTIMINGTOOL_IPV4PARSER_H
