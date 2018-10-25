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
    bool takeTimeOnWrite = true;
    uint64_t write_tstamp = 0;
    uint64_t read_tstamp = 0;

public:
    CPUTimingSocket();
    ~CPUTimingSocket() override = default;
    void write(const void* data, size_t size) override;
    ssize_t read(void *buf, size_t size, bool blocking) override;
    uint64_t getLastMeasurement() override;
};


#endif //NETWORKTIMINGTOOL_CPUTIMINGSOCKET_H
