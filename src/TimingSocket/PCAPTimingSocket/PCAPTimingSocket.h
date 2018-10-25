//
// Created by malte on 21.10.18.
//

#ifndef NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H
#define NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H

#include "../TimingSocket.h"
#include "../../PcapWrapper/PcapWrapper.h"

class PCAPTimingSocket : public TimingSocket {
    std::unique_ptr<PcapWrapper> pcap;
    void init() override;
    bool pcapInititalized = false;
    struct timeval tx_timestamp;
    struct timeval rx_timestamp;

public:
    PCAPTimingSocket();
    ~PCAPTimingSocket() override = default;
    void initPcap(std::string device);
    void connect(std::string host, uint16_t port) override;
    void close() override;
    void write(const void* data, size_t size) override;
    ssize_t read(void *buf, size_t size, bool blocking) override;
    uint64_t getLastMeasurement() override;
};


#endif //NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H
