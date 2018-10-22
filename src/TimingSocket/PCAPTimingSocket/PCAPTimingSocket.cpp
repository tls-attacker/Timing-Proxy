//
// Created by malte on 21.10.18.
//

#include "PCAPTimingSocket.h"
#include <arpa/inet.h>
#include <iostream>

void PCAPTimingSocket::init() {
}

PCAPTimingSocket::PCAPTimingSocket() {
    init();
}

bool isValidIpv4Address(const char *ipAddress)
{
    struct in_addr sin_addr;
    int result = inet_pton(AF_INET, ipAddress, &(sin_addr));
    return result != 0;
}

bool isValidIpv6Address(const char *ipAddress)
{
    struct in6_addr sin6_addr;
    int result = inet_pton(AF_INET6, ipAddress, &(sin6_addr));
    return result != 0;
}

void PCAPTimingSocket::connect(std::string host, uint16_t port) {
    if (!isValidIpv4Address(host.c_str()) && !isValidIpv6Address(host.c_str())){
        throw std::invalid_argument("Current pcap implementation needs an ip address in order to correlate packets.");
    }
    if (!pcapInititalized) {
        std::cout << "PCAP was not initialized. Using default interface as a fallback." << std::endl;
        std::make_unique<PcapWrapper>();
    }
    pcap->setFilter(host.c_str(), port);
    pcap->startLoop();
    TimingSocket::connect(host, port);
}

void PCAPTimingSocket::close() {
    pcap->stopLoop();
    TimingSocket::close();
}

void PCAPTimingSocket::initPcap(std::string device) {
    pcap = std::make_unique<PcapWrapper>(device.c_str());
    pcapInititalized = true;
}

void PCAPTimingSocket::write(const void *data, size_t size) {
    TimingSocket::write(data, size);
    tx_timestamp = pcap->timingForPacket(data, size, PcapLoopCallback::PacketDirection::DESTINATION_REMOTE);
}

ssize_t PCAPTimingSocket::read(void *buf, size_t size, bool blocking) {
    ssize_t bytes_read = TimingSocket::read(buf, size, blocking);
    if (bytes_read > 0) {
        rx_timestamp = pcap->timingForPacket(buf, (size_t)bytes_read, PcapLoopCallback::PacketDirection::SOURCE_REMOTE);
    }
    return bytes_read;
}

uint64_t PCAPTimingSocket::getLastMeasurement() {
    return PcapLoopCallback::timevalDeltaToNs(pcap->getPrecision(), &tx_timestamp, &rx_timestamp);
}
