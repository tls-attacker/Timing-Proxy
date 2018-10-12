//
// Created by Malte Poll on 18.09.18.
//

#ifndef PCAP_WRAPPER_H
#define PCAP_WRAPPER_H

#include <pcap.h>


class PcapWrapper {
    char errbuf[PCAP_ERRBUF_SIZE];
    const char* device;
    pcap_t *handle;
    struct bpf_program fp;

    void init();

public:
    PcapWrapper();
    PcapWrapper(const char* device);

    void setFilter(const char* remote_host, int remote_port);
};


#endif //PCAP_WRAPPER_H
