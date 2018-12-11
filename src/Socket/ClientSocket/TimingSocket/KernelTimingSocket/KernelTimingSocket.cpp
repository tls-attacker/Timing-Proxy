//
// Created by malte on 21.10.18.
//

#include <iostream>
#include <netinet/in.h>
#include "KernelTimingSocket.h"
#include <strings.h>
#include <cstring>
#include <sys/socket.h>
#include "../../../../PacketParser/PacketParser.h"
#include "../../../../PacketParser/Artefacts/Artefacts.h"
#include <chrono>
#include <thread>

uint64_t timespecDiff(struct timespec after, struct timespec before) {
    if (after.tv_sec < before.tv_sec || (after.tv_sec == before.tv_sec && after.tv_nsec < before.tv_nsec)) {
        throw;
    }

    uint64_t secDiff = uint64_t (after.tv_sec - before.tv_sec);
    if (after.tv_nsec < before.tv_nsec) {
        after.tv_nsec += 1000000000;
        secDiff-= 1;
    }
    return (secDiff*1000000000) + (after.tv_nsec - before.tv_nsec);
}

void Socket::KernelTimingSocket::init() {
    bzero(&config, sizeof(hwtstamp_config));
    bzero(&interface_request, sizeof(ifreq));
}

Socket::KernelTimingSocket::KernelTimingSocket() {
    init();
}

void Socket::KernelTimingSocket::connect(std::string host, uint16_t port) {
    if (host == "127.0.0.1" || host == "::1" || device == "lo") {
        throw std::invalid_argument("kernel timestamping on loopback does not work");
    }
    TimingSocket::connect(host, port);

    initializeDeviceForHardwareTimestamping();
    initializeSocketForHardwareTimestamping();
}

void Socket::KernelTimingSocket::write(const void *data, size_t size) {
    TimingSocket::write(data, size);
    getTxTimestamp(data, size);
}


uint64_t Socket::KernelTimingSocket::getLastMeasurement() {
    return timespecDiff(rx_timestamp, tx_timestamp);
}

void Socket::KernelTimingSocket::getTxTimestamp(const void *data, size_t size) {
    bool found_message_in_errqueue = false;
    size_t retries = 0;
    struct timespec* ts_ptr = nullptr;
    for (size_t retries = 0; !found_message_in_errqueue && retries < TIMESTAMPING_MESSAGE_MAX_RETRIES; ++retries) {
        ssize_t rc;
        struct iovec vec[1];
        struct msghdr msg;
        char data_buf[1500];
        struct cmsghdr *cmsg;

        union {
            struct cmsghdr cm;
            char control[256];
        } cmsg_un;

        vec[0].iov_base = data_buf;
        vec[0].iov_len = sizeof(data_buf);

        bzero(&msg, sizeof(msg));
        bzero(&cmsg_un, sizeof(cmsg_un));

        msg.msg_name = nullptr;
        msg.msg_namelen = 0;
        msg.msg_iov = vec;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsg_un.control;
        msg.msg_controllen = sizeof(cmsg_un.control);
        rc = recvmsg(sock, &msg, MSG_ERRQUEUE);
        using namespace std::chrono_literals;
        if (rc < 0 && errno == EAGAIN) {
            if (errno == EAGAIN) {
                std::this_thread::sleep_for(1ms);
                continue;
            }else{
                throw;
            }
        }
        if (msg.msg_flags & MSG_TRUNC) {
            printf("received truncated message\n");
            continue;
        }

        if (msg.msg_flags & MSG_CTRUNC) {
            printf("received truncated ancillary data\n");
            continue;
        }

        if (msg.msg_controllen <= 0) {
            printf("received short ancillary data (%ld/%ld)\n",
                   (long)msg.msg_controllen, (long)sizeof(cmsg_un.control));
            continue;
        }

        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg->cmsg_level != SOL_SOCKET) {
            continue;
        }
        if (cmsg->cmsg_type != SO_TIMESTAMPING) {
            continue;
        }

        PacketParser::Artefacts artefacts;
        void* payload = nullptr;
        size_t payload_size = 0;
        PacketParser::decodeUntil(PacketParser::Layer::tcp, data_buf, rc, &payload, &payload_size, &artefacts);

        if (memcmp(payload, data, size) != 0) {
            std::cout << "Did not find the required package.. Searching on"<<std::endl;
            continue;
        }

        ts_ptr = (struct timespec*) CMSG_DATA(cmsg);
        found_message_in_errqueue = true;
    }
    if (!found_message_in_errqueue){
        throw std::runtime_error("Could not get TX timestamp from interface "+device);
    }
    tx_timestamp = {ts_ptr[tstamp_source].tv_sec, ts_ptr[tstamp_source].tv_nsec};
}

ssize_t Socket::KernelTimingSocket::read(void *buf, size_t size, bool blocking) {
    struct timespec* ts_ptr = nullptr;
    ssize_t rc;
    struct iovec vec[1];
    struct msghdr msg;
    struct cmsghdr *cmsg;

    union {
        struct cmsghdr cm;
        char control[256];
    } cmsg_un;

    vec[0].iov_base = buf;
    vec[0].iov_len = size;

    bzero(&msg, sizeof(msg));
    bzero(&cmsg_un, sizeof(cmsg_un));

    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    msg.msg_iov = vec;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_un.control;
    msg.msg_controllen = sizeof(cmsg_un.control);
    int flags = 0;
    if (!blocking) {
        flags = flags | MSG_DONTWAIT;
    }
    rc = recvmsg(sock, &msg, flags);
    if (rc < 0) {
        switch (errno) {
            case EAGAIN:
                return rc;
            case ECONNRESET:
                throw std::runtime_error("Connection reset");
            default:
                std::cerr<< "reading error: " << errno << std::endl;
                throw;
        }
    }
    if (msg.msg_flags & MSG_TRUNC) {
        printf("received truncated message\n");
        throw;
    }

    if (msg.msg_flags & MSG_CTRUNC) {
        printf("received truncated ancillary data\n");
        throw;
    }

    if (msg.msg_controllen <= 0) {
        printf("received short ancillary data (%ld/%ld)\n",
               (long)msg.msg_controllen, (long)sizeof(cmsg_un.control));
        throw;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg->cmsg_level != SOL_SOCKET) {
        throw;
    }
    if (cmsg->cmsg_type != SO_TIMESTAMPING) {
        throw;
    }

    ts_ptr = (struct timespec*) CMSG_DATA(cmsg);
    rx_timestamp = {ts_ptr[tstamp_source].tv_sec, ts_ptr[tstamp_source].tv_nsec};
    return rc;
}

void Socket::KernelTimingSocket::setDevice(std::string device) {
    this->device = device;
}

void Socket::KernelTimingSocket::initializeDeviceForHardwareTimestamping() {
    /* try to read the current configuration */
    bzero(&config, sizeof(config));
    bzero(&interface_request, sizeof(interface_request));
    strncpy(interface_request.ifr_name, device.c_str(), IFNAMSIZ);
    interface_request.ifr_data = (char*)&config;
    int ioctl_err = ioctl(sock, SIOCGHWTSTAMP, &interface_request);
    if (ioctl_err == 0 && config.tx_type == HWTSTAMP_TX_ON && config.rx_filter == HWTSTAMP_FILTER_ALL) {
        std::cout << "Driver already has hardware timestamping enabled" <<std::endl;
        deviceSupportsHardwareTimestamping = true;
        return;
    }

    /* enable hardware timestamping in the kernel driver */
    bzero(&config, sizeof(config));
    config.tx_type = HWTSTAMP_TX_ON;
    config.rx_filter = HWTSTAMP_FILTER_ALL;
    bzero(&interface_request, sizeof(interface_request));
    strncpy(interface_request.ifr_name, device.c_str(), IFNAMSIZ);
    interface_request.ifr_data = (char*)&config;
    ioctl_err = ioctl(sock, SIOCSHWTSTAMP, &interface_request);
    if (ioctl_err) {
        std::cerr << "Hardware Timestamping cannot be enabled for the device "<<interface_request.ifr_name<<" due to the following reason: "<<strerror(errno)<<std::endl;
        deviceSupportsHardwareTimestamping = false;
    }else{
        std::cout << "Enabled hardware timestamping for "<<device<<" in the driver"<<std::endl;
        deviceSupportsHardwareTimestamping = true;
    }
}

void Socket::KernelTimingSocket::initializeSocketForHardwareTimestamping() {
    /*
     * Try to set hardware timestamping first
     * If that fails, fall back to software timestamping
     * */
    int setsockopt_err;
    if (deviceSupportsHardwareTimestamping) {
        setsockopt_err = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &SO_TIMESTAMPING_OPTIONS_HARDWARE, sizeof(int));
    }else{
        setsockopt_err = 1;
    }

    if (setsockopt_err) {
        setsockopt_err = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &SO_TIMESTAMPING_OPTIONS_SOFTWARE, sizeof(int));
        if (setsockopt_err) {
            throw std::runtime_error("Unable to enable timestamping on the socket");
        }else{
            std::cout << "Using software timestamps on the socket." << std::endl;
            tstamp_source = SOFTWARE;
        }
    }else{
        std::cout << "Using hardware timestamps on the socket." << std::endl;
        tstamp_source = HARDWARE;
    }
}
