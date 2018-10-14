//
// Created by malte on 14.10.18.
//

#ifndef NETWORKTIMINGTOOL_PACKETPARSERTESTS_H
#define NETWORKTIMINGTOOL_PACKETPARSERTESTS_H

#include "../PacketParser.h"
#include <cstring>

class PacketParserTests {
public:
    const static int TESTCASE_AMOUNT = 1;
    bool TEST_tcp() {
        char test_frame_ipv4[] = "" \
        "\x00\x90\x4c\x0d\xf4\x3e\x44\x8a\x5b\x85\x0d\x6e\x08\x00\x45\x00" \
        "\x00\x50\x6c\xfe\x40\x00\x40\x06\xe7\xf0\xc0\xa8\xb2\x2c\xc0\xa8" \
        "\xb2\x3b\xc6\xc6\x05\x39\x24\x8a\x21\x48\x8e\x0c\xf3\x9b\x80\x18" \
        "\x00\xe5\xb8\xa2\x00\x00\x01\x01\x08\x0a\x1a\x60\x74\x1a\xd8\xd3" \
        "\x28\x65\x4a\x75\x73\x74\x20\x74\x65\x73\x74\x69\x6e\x67\x20\x74" \
        "\x63\x70\x20\x6f\x76\x65\x72\x20\x69\x70\x76\x34\x21\x0a";

        char* payload = nullptr;
        size_t payload_size;

        const size_t wanted_size = 28;
        char wanted[wanted_size+1] = "Just testing tcp over ipv4!\n";


        PacketParser::decodeUntil(PacketParser::Layer::tcp, test_frame_ipv4, sizeof(test_frame_ipv4), (void**)&payload, &payload_size);
        if(memcmp(wanted, payload, wanted_size) == 0 && payload_size == wanted_size) {
            return true;
        }
        return false;
    }

    int run_testcases() {
        int sum = 0;
        sum += TEST_tcp();
        return sum;
    }
};


#endif //NETWORKTIMINGTOOL_PACKETPARSERTESTS_H
