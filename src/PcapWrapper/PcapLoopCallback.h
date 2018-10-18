//
// Created by malte on 17.10.18.
//

#ifndef NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H
#define NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H

#include <pcap/pcap.h>
#include <string>
#include <mutex>


namespace PcapLoopCallback {
    void handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

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


    class UserData {
        /* The wanted members might change at runtime */
        /* They are protected by wanted_mutex */
        std::mutex wanted_mutex;
        const void * wanted_payload = nullptr;
        size_t wanted_payload_size = 0;
        PacketDirection wanted_packet_direction = DESTINATION_REMOTE;
        /* found_result and timing might change at runtime */
        std::mutex result_mutex;
        bool found_result = true;
        uint64_t timing = 0;
        uint64_t first_timestamp = 0;
        bool found_first = false;
    public:
        void lock_wanted();
        void unlock_wanted();
        void setWanted(const void* wanted_payload, size_t wanted_payload_size, PacketDirection wanted_packet_direction);
        bool hasResult();
        uint64_t waitForResult();
        void waitForNewWanted();
        void setResult(uint64_t result);

        /* These members are never accessed from more than one thread */
        LinkType linktype;
        pcap_t* handle; /* pcap handle */
        std::string remote_host;
        uint16_t remote_port;
        bool stop_loop = false;
        friend void handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
    };


};


#endif //NETWORKTIMINGTOOL_PCAPLOOPCALLBACK_H
