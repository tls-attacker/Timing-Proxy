//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_IPV6PARSER_H
#define NETWORKTIMINGTOOL_IPV6PARSER_H

#include "../PacketParser.h"
#include <linux/in6.h>
#include <map>

class Ipv6Parser : public PacketParser {
    static const size_t MAIN_HEADER_SIZE = 40;
    static const size_t EXT_HEADER_SIZE = 8;
    struct ipv6_header {
        uint32_t ver_trafcl_flow;
        uint16_t payload_len;
        uint8_t next_hdr;
        uint8_t hop_limit;
        in6_addr addr_src;
        in6_addr addr_dst;
    };

    struct ipv6_ext_header {
        uint8_t next_hdr;
        uint8_t hdr_len;
    };

    enum NextHeaderType {
        hopopt     = 0x00,
        tcp        = 0x06,
        udp        = 0x11,
        ipv6_route = 0x2b,
        ipv6_frag  = 0x2c,
        esp        = 0x32,
        ah         = 0x33,
        ipv6_nonxt = 0x3b,
        ipv6_opts  = 0x3c,
        mobility   = 0x87,
        hip        = 0x8b,
        shim6      = 0x8c,
        tst1       = 0xfd,
        tst2       = 0xfe,
    };

    enum NextHeaderKind {
        hk_not_implemented = 0,
        hk_payload,
        hk_extension_header,
        hk_no_next,
    };

    const std::map<enum NextHeaderType, NextHeaderKind> HeaderTypeMap = {
            {hopopt, hk_extension_header},
            {tcp, hk_payload},
            {udp, hk_payload},
            {ipv6_route, hk_extension_header},
            {ipv6_frag, hk_extension_header},
            {esp, hk_extension_header},
            {ah, hk_extension_header},
            {ipv6_nonxt, hk_no_next},
            {ipv6_opts, hk_extension_header},
            {mobility, hk_extension_header},
            {hip, hk_extension_header},
            {shim6, hk_extension_header},
            {tst1, hk_extension_header},
            {tst2, hk_extension_header},
    };

    uint8_t hop_limit = 0;
    uint8_t next_hdr;
    const in6_addr* addr_src;
    const in6_addr* addr_dst;
    bool isExtensionHeader(uint8_t next_hdr);
public:
    void parseHeader(void* packet, size_t size);
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size);
};


#endif //NETWORKTIMINGTOOL_IPV6PARSER_H
