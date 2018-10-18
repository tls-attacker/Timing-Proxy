//
// Created by malte on 17.10.18.
//

#ifndef NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H
#define NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H

#include <pcap/pcap.h>
#include <string>


namespace PcapLoopCallback {
    /*
     * PCAP Link Types. Only supported types are listed
     * See https://www.tcpdump.org/linktypes/ for a complete list
     * */
    enum LinkType {
        LINKTYPE_NULL = 0,
        LINKTYPE_ETHERNET = 1,
        LINKTYPE_LINUX_SLL = 113,
    };


    class UserData {
    public:
        LinkType linktype;
        // TODO: Add search function callback
        pcap_t* handle; /* pcap handle */
        std::string remote_host;
        uint16_t remote_port;
    };

    void handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
};


#endif //NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H
