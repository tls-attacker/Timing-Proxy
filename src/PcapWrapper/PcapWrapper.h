//
// Created by Malte Poll on 18.09.18.
//

#ifndef PCAP_WRAPPER_H
#define PCAP_WRAPPER_H

#include <pcap.h>
#include <unistd.h>
#include <thread>
#include "PcapLoopCallback.h"


class PcapWrapper {
    char errbuf[PCAP_ERRBUF_SIZE];
    const char* device;
    pcap_t *handle;
    PcapLoopCallback::LinkType linktype;
    PcapLoopCallback::UserData usrdata;
    struct bpf_program fp;
    std::thread *loop_thread = nullptr;
    void init();

public:
    PcapWrapper();
    PcapWrapper(const char* device);

    void setFilter(const char* remote_host, uint16_t remote_port);
    void startLoop();
    void stopLoop();
    int getPrecision();
    struct timeval timingForPacket(const void* buf, size_t buflen, PcapLoopCallback::PacketDirection direction = PcapLoopCallback::PacketDirection::DESTINATION_REMOTE);
};


#endif //PCAP_WRAPPER_H
