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

public:
    PCAPTimingSocket();
    void initPcap(std::string device);
    void connect(std::string host, uint16_t port) override;
    void close() override;
    uint64_t writeAndTimeResponse(const void *data, size_t size) override;
};


#endif //NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H
