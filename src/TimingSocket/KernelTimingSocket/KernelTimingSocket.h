//
// Created by malte on 21.10.18.
//

#ifndef NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H
#define NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H

#include "../TimingSocket.h"
#include <sys/socket.h>
#include <linux/net_tstamp.h>
#include <linux/sockios.h>

static const uint32_t SO_TIMESTAMPING_OPTIONS_HARDWARE = SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE;
static const uint32_t SO_TIMESTAMPING_OPTIONS_SOFTWARE = SOF_TIMESTAMPING_RX_SOFTWARE | SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;

class KernelTimingSocket : public TimingSocket{
    enum TSTAMP_SOURCE {
        SOFTWARE = 0,
        HARDWARE = 2,
    };
    TSTAMP_SOURCE tstamp_source;
    void init() override;
    struct timespec writeAndGetTimestamp(const void* data, size_t size);
    struct timespec getRxTimestamp();

public:
    KernelTimingSocket();
    void connect(std::string host, uint16_t port) override;
    void write(const void* data, size_t size) override;
    uint64_t writeAndTimeResponse(const void *data, size_t size) override;
};


#endif //NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H
