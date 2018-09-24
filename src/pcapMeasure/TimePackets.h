//
// Created by Malte Poll on 18.09.18.
//

#ifndef TIMING_TIMEPACKETS_H
#define TIMING_TIMEPACKETS_H

#include <pcap.h>


class TimePackets {
    char errbuf[PCAP_ERRBUF_SIZE];
    char* device;
    pcap_t *handle;
    struct bpf_program fp;

    void init();

public:
    TimePackets();
    TimePackets(char* device);

    void setFilter(char* remote_host, int remote_port);
};


#endif //TIMING_TIMEPACKETS_H
