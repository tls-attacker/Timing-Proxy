//
// Created by malte on 14.10.18.
//
#include "../../src/PacketParser/PacketParser.h"
#include "../../src/PacketParser/Ethernet/EthernetParser.h"
#include "../../src/PacketParser/Ipv4/Ipv4Parser.h"
#include "../../src/PacketParser/Ipv6/Ipv6Parser.h"
#include "../../src/PacketParser/Tcp/TcpParser.h"
#include "../../src/PacketParser/Udp/UdpParser.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include <stdexcept>
#include <cstring>



class PacketParserDecodeTest : public ::testing::Test {
protected:
    void * payload = nullptr;
    size_t payload_size = 0;
    static constexpr size_t want_size = 28;
    char want[want_size];
    virtual void SetUp() {

    }

    virtual void TearDown() {
        EXPECT_THAT(std::vector<char>((char*)payload, (char*)payload + payload_size),
                    ::testing::ElementsAreArray(want));
        EXPECT_EQ(payload_size, want_size);
    }
};

TEST_F(PacketParserDecodeTest, EthernetIpv4TcpDecode) {
    const char raw_packet[] = "\x00\x90\x4c\x0d\xf4\x3e\x44\x8a\x5b\x85\x0d\x6e\x08\x00\x45\x00" \
        "\x00\x50\x6c\xfe\x40\x00\x40\x06\xe7\xf0\xc0\xa8\xb2\x2c\xc0\xa8" \
        "\xb2\x3b\xc6\xc6\x05\x39\x24\x8a\x21\x48\x8e\x0c\xf3\x9b\x80\x18" \
        "\x00\xe5\xb8\xa2\x00\x00\x01\x01\x08\x0a\x1a\x60\x74\x1a\xd8\xd3" \
        "\x28\x65\x4a\x75\x73\x74\x20\x74\x65\x73\x74\x69\x6e\x67\x20\x74" \
        "\x63\x70\x20\x6f\x76\x65\x72\x20\x69\x70\x76\x34\x21\x0a";
    PacketParser::decodeUntil(PacketParser::Layer::tcp, (void*)raw_packet, sizeof(raw_packet)-1, &payload,
                              &payload_size);
    char want[want_size] = {'J', 'u', 's', 't', ' ', 't', 'e', 's', 't', 'i', 'n', 'g',
                                  ' ', 't', 'c', 'p', ' ', 'o', 'v', 'e', 'r', ' ', 'i', 'p',
                                  'v', '4', '!', '\n'};
    memcpy(this->want, want, want_size);
}


TEST_F(PacketParserDecodeTest, EthernetIpv6TcpDecode) {
    const char raw_packet[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x86\xdd\x60\x04" \
        "\xb1\xa0\x00\x3c\x06\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x01\xa9\x28\x05\x39\xa8\x3c\x83\x9d\xe2\x96" \
        "\xcb\x63\x80\x18\x01\x56\x00\x44\x00\x00\x01\x01\x08\x0a\x9d\x4d" \
        "\xf9\x64\x9d\x4d\xe9\xa3\x4a\x75\x73\x74\x20\x74\x65\x73\x74\x69" \
        "\x6e\x67\x20\x74\x63\x70\x20\x6f\x76\x65\x72\x20\x69\x70\x76\x36" \
        "\x21\x0a";
    PacketParser::decodeUntil(PacketParser::Layer::tcp, (void*)raw_packet, sizeof(raw_packet)-1, &payload,
                              &payload_size);
    char want[want_size] = {'J', 'u', 's', 't', ' ', 't', 'e', 's', 't', 'i', 'n', 'g',
                            ' ', 't', 'c', 'p', ' ', 'o', 'v', 'e', 'r', ' ', 'i', 'p',
                            'v', '6', '!', '\n'};
    memcpy(this->want, want, want_size);
}

TEST_F(PacketParserDecodeTest, EthernetIpv4UdpDecode) {
    const char raw_packet[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x45\x00" \
        "\x00\x38\xa8\xb1\x40\x00\x40\x11\x94\x01\x7f\x00\x00\x01\x7f\x00" \
        "\x00\x01\x92\xbe\x05\x39\x00\x24\xfe\x37\x4a\x75\x73\x74\x20\x74" \
        "\x65\x73\x74\x69\x6e\x67\x20\x75\x64\x70\x20\x6f\x76\x65\x72\x20" \
        "\x69\x70\x76\x34\x21\x0a";
    PacketParser::decodeUntil(PacketParser::Layer::udp, (void*)raw_packet, sizeof(raw_packet)-1, &payload,
                              &payload_size);
    char want[want_size] = {'J', 'u', 's', 't', ' ', 't', 'e', 's', 't', 'i', 'n', 'g',
                            ' ', 'u', 'd', 'p', ' ', 'o', 'v', 'e', 'r', ' ', 'i', 'p',
                            'v', '4', '!', '\n'};
    memcpy(this->want, want, want_size);
}

TEST_F(PacketParserDecodeTest, EthernetIpv6UdpDecode) {
    const char raw_packet[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x86\xdd\x60\x0b" \
        "\x2a\x74\x00\x24\x11\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
        "\x00\x00\x00\x00\x00\x01\xe2\x84\x05\x39\x00\x24\x00\x37\x4a\x75" \
        "\x73\x74\x20\x74\x65\x73\x74\x69\x6e\x67\x20\x75\x64\x70\x20\x6f" \
        "\x76\x65\x72\x20\x69\x70\x76\x36\x21\x0a";
    PacketParser::decodeUntil(PacketParser::Layer::udp, (void*)raw_packet, sizeof(raw_packet)-1, &payload,
                              &payload_size);
    char want[want_size] = {'J', 'u', 's', 't', ' ', 't', 'e', 's', 't', 'i', 'n', 'g',
                            ' ', 'u', 'd', 'p', ' ', 'o', 'v', 'e', 'r', ' ', 'i', 'p',
                            'v', '6', '!', '\n'};
    memcpy(this->want, want, want_size);
}

class PacketParserDecodeExceptions : public ::testing::Test {
protected:
    char * raw_packet = nullptr;
    size_t raw_packet_size = 0;
    void * payload = nullptr;
    size_t payload_size = 0;
    PacketParser* pp = nullptr;
    virtual void SetUp() {

    }

    void parse() {
        EXPECT_THROW(pp->parseHeader(raw_packet, raw_packet_size), std::invalid_argument);
    }

    virtual void TearDown() {
        free(raw_packet);
    }
};

TEST_F(PacketParserDecodeExceptions, ParseHeaderEthernetThrows) {
    const char raw_packet[] = {'\x00'};
    raw_packet_size = 1;
    this->raw_packet = (char*)malloc(raw_packet_size);
    memcpy(this->raw_packet, raw_packet, raw_packet_size);
    pp = new EthernetParser;
    parse();
};

TEST_F(PacketParserDecodeExceptions, ParseHeaderIpv4Throws) {
    const char raw_packet[] = {'\x00'};
    raw_packet_size = 1;
    this->raw_packet = (char*)malloc(raw_packet_size);
    memcpy(this->raw_packet, raw_packet, raw_packet_size);
    pp = new Ipv4Parser;
    parse();
};

TEST_F(PacketParserDecodeExceptions, ParseHeaderIpv6Throws) {
    const char raw_packet[] = {'\x00'};
    raw_packet_size = 1;
    this->raw_packet = (char*)malloc(raw_packet_size);
    memcpy(this->raw_packet, raw_packet, raw_packet_size);
    pp = new Ipv6Parser;
    parse();
};

TEST_F(PacketParserDecodeExceptions, ParseHeaderTcpThrows) {
    const char raw_packet[] = {'\x00'};
    raw_packet_size = 1;
    this->raw_packet = (char*)malloc(raw_packet_size);
    memcpy(this->raw_packet, raw_packet, raw_packet_size);
    pp = new TcpParser;
    parse();
};

TEST_F(PacketParserDecodeExceptions, ParseHeaderUdpThrows) {
    const char raw_packet[] = {'\x00'};
    raw_packet_size = 1;
    this->raw_packet = (char*)malloc(raw_packet_size);
    memcpy(this->raw_packet, raw_packet, raw_packet_size);
    pp = new UdpParser;
    parse();
};