//
// Created by malte on 21.10.18.
//

#ifndef NETWORKTIMINGTOOL_CPUTIMINGSOCKET_H
#define NETWORKTIMINGTOOL_CPUTIMINGSOCKET_H

#include "../TimingSocket.h"
#include "../../TimeSources/TimeSources.h"

class CPUTimingSocket : public TimingSocket {
    uint64_t (*best_timesource)() = nullptr;
    TimeSources::cpu_features cpu_features;
    void init() override;

public:
    CPUTimingSocket();
    uint64_t writeAndTimeResponse(const void *data, size_t size) override;
};


#endif //NETWORKTIMINGTOOL_CPUTIMINGSOCKET_H
