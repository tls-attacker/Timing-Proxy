//
// Created by malte on 17.10.18.
//

#ifndef NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H
#define NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H

#include <pcap/pcap.h>
#include <string>
#include <mutex>


namespace PcapLoopCallback {
    const size_t MAX_PACKET_SIZE = 1500;
    const size_t SHARED_BUFFER_SIZE = 1000;

    void handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
    uint64_t timevalDeltaToNs(int tstamp_precision, const struct timeval* tv_early, const struct timeval* tv_late);

    /*
     * PCAP Link Types. Only supported types are listed
     * See https://www.tcpdump.org/linktypes/ for a complete list
     * */
    enum LinkType {
        LINKTYPE_NULL = 0,
        LINKTYPE_ETHERNET = 1,
        LINKTYPE_LINUX_SLL = 113,
    };

    enum PacketDirection {
        DESTINATION_REMOTE,
        SOURCE_REMOTE,
    };

    struct PacketInfo {
        char payload[MAX_PACKET_SIZE];
        size_t payload_size;
        struct timeval timing;
        PacketDirection direction;
    };


    class UserData {
    public:
        /* These members are never accessed from more than one thread */
        struct PcapLoopCallback::PacketInfo shared_buffer_a[PcapLoopCallback::SHARED_BUFFER_SIZE];
        struct PcapLoopCallback::PacketInfo shared_buffer_b[PcapLoopCallback::SHARED_BUFFER_SIZE];
        bool active_buffer_consumer = 0;
        bool active_buffer_producer = 0;
        size_t shared_buffer_index_producer = 0; // index used by the callback
        size_t shared_buffer_index_consumer = 0; // index used by the pcapWrapper
        LinkType linktype;
        pcap_t* handle; /* pcap handle */
        int tstamp_precision;
        std::string remote_host;
        uint16_t remote_port;
        bool stop_loop = false;
        friend void handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
    };


};


#endif //NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H
