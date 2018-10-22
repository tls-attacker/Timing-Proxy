//
// Created by malte on 21.10.18.
//

#ifndef NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H
#define NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H

#include "../TimingSocket.h"
#include "../../PcapWrapper/PcapWrapper.h"

class PCAPTimingSocket : public TimingSocket {
    PcapWrapper pcap;
    void init() override;

public:
    PCAPTimingSocket();
    void connect(std::string host, uint16_t port) override;
    void close() override;
    uint64_t writeAndTimeResponse(const void *data, size_t size) override;
};


#endif //NETWORKTIMINGTOOL_PCAPTIMINGSOCKET_H
