//
// Created by malte on 17.10.18.
//

#include <iostream>
#include "PcapLoopCallback.h"
#include "../PacketParser/PacketParser.h"
#include "../PacketParser/Artefacts/Artefacts.h"
#include "../PacketParser/Ethernet/EthernetParser.h"
#include "../PacketParser/Ipv4/Ipv4Parser.h"
#include "../PacketParser/Ipv6/Ipv6Parser.h"
#include "../PacketParser/Tcp/TcpParser.h"
#include "../PacketParser/Udp/UdpParser.h"
#include <thread>
#include <cstring>

using namespace std::chrono_literals;

uint64_t PcapLoopCallback::timevalDeltaToNs(int tstamp_precision, const struct timeval* tv_early, const struct timeval* tv_late) {
    /* HACK for non monoton timestamps */
    long* late_secs = (long*)&tv_late->tv_sec;
    (*late_secs) += 3;
    /* END HACK */

    uint64_t usecs_per_second = 0;
    if (tstamp_precision == PCAP_TSTAMP_PRECISION_MICRO) {
        usecs_per_second =    1000000;
    }else if(tstamp_precision == PCAP_TSTAMP_PRECISION_NANO){
        usecs_per_second = 1000000000;
    } else {
        throw;
    }
    if (tv_early->tv_sec > tv_late->tv_sec) {
        throw;
    }

    uint64_t seconds = 0;
    uint64_t usecs = 0;

    seconds =(uint64_t) (tv_late->tv_sec - tv_early->tv_sec);
    if (tv_late->tv_usec < tv_early->tv_usec) {
        if (seconds == 0) {
            throw;
        }
        seconds--;
        usecs += usecs_per_second;
    }
    usecs = ((uint64_t)tv_late->tv_usec + usecs)-(uint64_t)tv_early->tv_usec;

    if (tstamp_precision == PCAP_TSTAMP_PRECISION_MICRO) {
        return seconds*1000000000 + usecs*1000;
    }else if(tstamp_precision == PCAP_TSTAMP_PRECISION_NANO){
        return seconds*1000000000 + usecs;
    } else {
        throw;
    }
}

void PcapLoopCallback::handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    //std::cerr << "Callback called!" << std::endl;
    auto usrdata = (UserData*)args;
    if (usrdata->stop_loop) {
        return;
    }
    const u_char * payload = nullptr;
    size_t payload_size = 0;
    PacketParser::Artefacts artefacts;
    try {
        switch (usrdata->linktype) {
            case LINKTYPE_ETHERNET:
                PacketParser::decodeUntil(PacketParser::Layer::tcp, (void*)packet, header->len, (void**)&payload, &payload_size, &artefacts);
                break;
            default:
                throw std::invalid_argument("Link type not implemented");
                break;
        }
    } catch (const std::invalid_argument &exception) {
        std::cerr << "Unable to decode packet!: " << exception.what() << std::endl;
        return;
    }

    if (payload_size > 1500) {
        throw std::invalid_argument("Payload size greater 1500 bytes is not supported");
    }

    if (payload_size == 0) {
        //std::cerr << "Empty packet!" << std::endl;
        return;
    }

    //std::cerr << "Got a payload!" << std::endl << payload_size << std::endl;

    PacketDirection direction;
    if(usrdata->remote_host == std::string(artefacts.dst_ip) && usrdata->remote_port == artefacts.dst_port) {
        direction = DESTINATION_REMOTE;
    }else{
        direction = SOURCE_REMOTE;
    }

    if (usrdata->shared_buffer_index_producer == SHARED_BUFFER_SIZE) {
        // switch buffer if consumer is done with other buffer
        // else panic (for now at least)
        if (usrdata->active_buffer_producer == usrdata->active_buffer_consumer) {
            usrdata->shared_buffer_index_producer = 0;
            usrdata->active_buffer_producer = !usrdata->active_buffer_producer;
        } else {
            throw;
        }
    }

    struct PacketInfo* active_buffer = usrdata->active_buffer_producer ? usrdata->shared_buffer_b : usrdata->shared_buffer_a;
    struct PacketInfo& pinfo = active_buffer[usrdata->shared_buffer_index_producer];
    memcpy(pinfo.payload, payload, payload_size);
    pinfo.payload_size = payload_size;
    pinfo.direction = direction;
    pinfo.timing.tv_sec = header->ts.tv_sec;
    pinfo.timing.tv_usec = header->ts.tv_usec;
    (usrdata->shared_buffer_index_producer)++;
    //std::cerr << "Callback finished!" << std::endl;
}