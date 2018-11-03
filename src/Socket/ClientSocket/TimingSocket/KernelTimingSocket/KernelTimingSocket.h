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

namespace Socket{
    class KernelTimingSocket : public Socket::TimingSocket{
        static const size_t TIMESTAMPING_MESSAGE_MAX_RETRIES = 1000;
        enum TSTAMP_SOURCE {
            SOFTWARE = 0,
            HARDWARE = 2,
        };
        TSTAMP_SOURCE tstamp_source;
        std::string device;
        hwtstamp_config config;
        ifreq interface_request;
        bool deviceSupportsHardwareTimestamping = false;
        void initializeDeviceForHardwareTimestamping();
        void initializeSocketForHardwareTimestamping();
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
        void setDevice(std::string device);
    };
}

#endif //NETWORKTIMINGTOOL_KERNELTIMINGSOCKET_H
