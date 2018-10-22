//
// Created by malte on 21.10.18.
//

#include <iostream>
#include <netinet/in.h>
#include "KernelTimingSocket.h"
#include <strings.h>
#include <cstring>
#include <sys/socket.h>
#include "../../PacketParser/PacketParser.h"
#include "../../PacketParser/Artefacts/Artefacts.h"

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

void KernelTimingSocket::init() {

}

KernelTimingSocket::KernelTimingSocket() {
    init();
}

uint64_t KernelTimingSocket::writeAndTimeResponse(const void *data, size_t size) {
    timespec tx_timestamp = writeAndGetTimestamp(data, size);
    timespec rx_timestamp = getRxTimestamp();
    return timespecDiff(rx_timestamp, tx_timestamp);
}

void KernelTimingSocket::connect(std::string host, uint16_t port) {
    TimingSocket::connect(host, port);
    /*
     * Try to set hardware timestamping first
     * If that fails, fall back to software timestamping
     * */
    int err = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &SO_TIMESTAMPING_OPTIONS_HARDWARE, sizeof(int));
    if (err) {
        err = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &SO_TIMESTAMPING_OPTIONS_SOFTWARE, sizeof(int));
        if (err) {
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

void KernelTimingSocket::write(const void *data, size_t size) {
    writeAndGetTimestamp(data, size);
}

struct timespec KernelTimingSocket::writeAndGetTimestamp(const void *data, size_t size) {
    TimingSocket::write(data, size);
    bool found_message_in_errqueue = false;
    struct timespec* ts_ptr = nullptr;
    while (!found_message_in_errqueue) {
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
        if (rc < 0 && errno == EAGAIN) {
            if (errno == EAGAIN) {
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
            printf("`received short ancillary data (%ld/%ld)`\n",
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
    return {ts_ptr[tstamp_source].tv_sec, ts_ptr[tstamp_source].tv_nsec};
}

struct timespec KernelTimingSocket::getRxTimestamp() {
    bool got_message = false;
    struct timespec* ts_ptr = nullptr;
    while (!got_message) {
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
        rc = recvmsg(sock, &msg, MSG_PEEK);
        if (rc < 0) {
            if (errno == EAGAIN) {
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
            printf("`received short ancillary data (%ld/%ld)`\n",
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

        ts_ptr = (struct timespec*) CMSG_DATA(cmsg);
        got_message = true;
    }
    return {ts_ptr[tstamp_source].tv_sec, ts_ptr[tstamp_source].tv_nsec};

}
