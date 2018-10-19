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

uint64_t convertPcapTimestampToNs(pcap_t* handle, const struct timeval* tv) {
    int precision = pcap_get_tstamp_precision(handle);
    if (precision == PCAP_TSTAMP_PRECISION_MICRO) {
        return ((uint64_t)tv->tv_sec)*1000000000 + ((uint64_t)tv->tv_usec)*1000;
    }else{
        return ((uint64_t)tv->tv_sec)*1000000000 + ((uint64_t)tv->tv_usec);
    }
}

void PcapLoopCallback::handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    auto usrdata = (UserData*)args;
    usrdata->waitForNewWanted();
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

    PacketDirection direction;
    if(usrdata->remote_host == std::string(artefacts.dst_ip) && usrdata->remote_port == artefacts.dst_port) {
        direction = DESTINATION_REMOTE;
    }else{
        direction = SOURCE_REMOTE;
    }



    if (usrdata->found_first) {
        usrdata->lock_wanted();
        if (direction != usrdata->wanted_packet_direction && payload_size > 0) {
            usrdata->setResult(convertPcapTimestampToNs(usrdata->handle, &header->ts) - usrdata->first_timestamp);
        }
        usrdata->unlock_wanted();
        return;
    }

    /* we will now lock the wanted information to make sure the main thread doesn't free it */
    usrdata->lock_wanted();
    try {

        if (direction != usrdata->wanted_packet_direction) {
            throw std::invalid_argument("Wrong packet direction");
        }
        if (payload_size != usrdata->wanted_payload_size) {
            throw std::invalid_argument("Wrong packet size");
        }
        if (memcmp(payload, usrdata->wanted_payload, usrdata->wanted_payload_size) != 0) {
            throw std::invalid_argument("Wrong packet content");
        }
        usrdata->found_first = true;
        usrdata->first_timestamp = convertPcapTimestampToNs(usrdata->handle, &header->ts);

        usrdata->unlock_wanted();
    }catch (const std::invalid_argument &exception){
        usrdata->unlock_wanted();
    }
}


void PcapLoopCallback::UserData::lock_wanted() {
    wanted_mutex.lock();
}

void PcapLoopCallback::UserData::unlock_wanted() {
    wanted_mutex.unlock();
}

void PcapLoopCallback::UserData::setWanted(const void* wanted_payload, size_t wanted_payload_size, PacketDirection wanted_packet_direction) {
    lock_wanted();
    this->wanted_payload = wanted_payload;
    this->wanted_payload_size = wanted_payload_size;
    this->wanted_packet_direction = wanted_packet_direction;
    unlock_wanted();
    result_mutex.lock();
    timing = 0;
    first_timestamp = 0;
    found_first = false;
    found_result = false;
    result_mutex.unlock();
}

bool PcapLoopCallback::UserData::hasResult() {
    result_mutex.lock();
    bool found_result = this->found_result;
    result_mutex.unlock();
    return found_result;
}

uint64_t PcapLoopCallback::UserData::waitForResult() {
    bool found_result = false;
    result_mutex.lock();
    while (!found_result) {
        found_result = this->found_result;
        result_mutex.unlock();
        if (!found_result) {
            //std::this_thread::sleep_for(10ms);
        }
        result_mutex.lock();
    }
    uint64_t timing = this->timing;
    result_mutex.unlock();
    return timing;
}

void PcapLoopCallback::UserData::waitForNewWanted() {
    bool found_result = hasResult();
    result_mutex.lock();
    while (found_result) {
        found_result = this->found_result;
        result_mutex.unlock();
        if (found_result) {
            //std::this_thread::sleep_for(10ms);
        }
        if (stop_loop) {
            return;
        }
        result_mutex.lock();
    }
    result_mutex.unlock();
}

void PcapLoopCallback::UserData::setResult(uint64_t result) {
    result_mutex.lock();
    timing = result;
    found_result = true;
    result_mutex.unlock();
}