//
// Created by malte on 21.10.18.
//

#ifndef NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H
#define NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H

#include "../TimingSocket.h"
#include <sys/socket.h>
#include <linux/net_tstamp.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <net/if.h>

static const uint32_t SO_TIMESTAMPING_OPTIONS_HARDWARE = SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE;
static const uint32_t SO_TIMESTAMPING_OPTIONS_SOFTWARE = SOF_TIMESTAMPING_RX_SOFTWARE | SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;

class KernelTimingSocket : public TimingSocket{
    enum TSTAMP_SOURCE {
        SOFTWARE = 0,
        HARDWARE = 2,
    };
    TSTAMP_SOURCE tstamp_source;
    hwtstamp_config config;
    ifreq interface_request;
    void init() override;
    void getTxTimestamp(const void *data, size_t size);
    void getRxTimestamp(const void *data, size_t size);
    timespec tx_timestamp;
    timespec rx_timestamp;

public:
    KernelTimingSocket();
    ~KernelTimingSocket() override = default;
    void connect(std::string host, uint16_t port) override;
    void write(const void* data, size_t size) override;
    ssize_t read(void *buf, size_t size, bool blocking) override;
    uint64_t getLastMeasurement() override;
    void enableHardwareTimestampingForDevice(std::string device);
};


#endif //NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H
