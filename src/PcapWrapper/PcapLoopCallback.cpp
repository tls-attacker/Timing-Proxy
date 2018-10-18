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

void PcapLoopCallback::handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    const UserData* usrdata = (UserData*)args;
    const u_char * payload = nullptr;
    size_t payload_size;
    PacketParser::Artefacts artefacts;
    try {
        switch (usrdata->linktype) {
            case LINKTYPE_ETHERNET:
                PacketParser::decodeUntil(PacketParser::Layer::tcp, (void*)packet, header->len, (void**)&payload, &payload_size, &artefacts);
                break;
            default:
                throw;
                break;
        }
    } catch (const std::exception &exception) {
        std::cout << "Unable to decode packet!" << std::endl;
    }
    if(!payload_size){
        return;
    }
    bool to_client = (usrdata->remote_host == std::string(artefacts.dst_ip) && usrdata->remote_port == artefacts.dst_port);
    if (to_client) {
        std::cout << ">";
    }else{
        std::cout << "<";
    }
    std::cout << header->ts.tv_sec << ":" <<header->ts.tv_usec <<" Got a packet from "<<artefacts.src_ip<<":"<<artefacts.src_port<<" to " << artefacts.dst_ip<<":"<<artefacts.dst_port <<" of size " << header->len << " with a tcp payload of size " << payload_size << std::endl;
    std::cout.write((char*)payload, payload_size);
    std::cout << std::endl;
}