//
// Created by malte on 21.10.18.
//

#include "PCAPTimingSocket.h"

void PCAPTimingSocket::init() {
}

PCAPTimingSocket::PCAPTimingSocket() {
    init();
}

uint64_t PCAPTimingSocket::writeAndTimeResponse(const void *data, size_t size) {
    write(data, size);
    return pcap.timingForPacket(data, size);
}

void PCAPTimingSocket::connect(std::string host, uint16_t port) {
    pcap.setFilter(host.c_str(), port);
    pcap.startLoop();
    TimingSocket::connect(host, port);
}

void PCAPTimingSocket::close() {
    pcap.stopLoop();
    TimingSocket::close();
}
