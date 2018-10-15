//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_TCPPARSER_H
#define NETWORKTIMINGTOOL_TCPPARSER_H

#include "../PacketParser.h"

class TcpParser : public PacketParser {
    const static size_t MIN_HEADER_LENGTH = 20;
    struct tcp_header {
        uint16_t th_sport; // source port
        uint16_t th_dport; // destination port
        uint32_t th_seq; // sequence number
        uint32_t th_ack; // acknowledgement number
        uint8_t th_offx2; // data offset, rsvd
        uint8_t th_flags; // collection of flags
        uint8_t th_win; // winodw
        uint8_t th_sum; // checksum
        uint8_t th_urp; // urgent pointer
    };
    uint16_t sport = 0;
    uint16_t dport = 0;
    uint32_t seq = 0;
    uint32_t ack = 0;
    size_t header_length = 0;
public:
    void parseHeader(void* packet, size_t size) override;
    static void decodeUntil(Layer layer, void* packet, size_t size, void** payload, size_t* payload_size);
};


#endif //NETWORKTIMINGTOOL_TCPPARSER_H
