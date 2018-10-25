//
// Created by Malte Poll on 18.09.18.
//

#include "PcapWrapper.h"
#include "../PacketParser/Tcp/TcpParser.h"

#include <netinet/in.h>
#include <string>
#include <iostream>
#include <cstring>

PCAP_API pcap_t * custom_pcap_open_live(const char *device, int snaplen, int promisc, int to_ms, char *errbuf){
    /* custom variables */
    const char *tstamp_type_name;
    int *tstamp_types = 0;
    int n_tstamp_types;
    /* end custom variables */
    
    pcap_t *p;
    int status;
    
    p = pcap_create(device, errbuf);
    if (p == NULL)
        return (NULL);
    
    /* custom code to set timing precision */
    n_tstamp_types = pcap_list_tstamp_types(p, &tstamp_types);
    
    if (n_tstamp_types < 0)
        fprintf(stderr ,"%s", pcap_geterr(p));
    
    if (n_tstamp_types == 0) {
        fprintf(stderr, "Time stamp type cannot be set for %s\n",
                device);
    }
    
    int best_timestamp_type_available = PCAP_TSTAMP_HOST;
    for (int i = 0; i < n_tstamp_types; i++) {
        tstamp_type_name = pcap_tstamp_type_val_to_name(tstamp_types[i]);
        if (tstamp_type_name != NULL) {
            (void) fprintf(stderr, "  %s (%s)\n", tstamp_type_name,
                           pcap_tstamp_type_val_to_description(tstamp_types[i]));
        } else {
            (void) fprintf(stderr, "  %d\n", tstamp_types[i]);
        }
        if (tstamp_types[i] > best_timestamp_type_available) {
            best_timestamp_type_available = tstamp_types[i];
        }
    }
    
    pcap_free_tstamp_types(tstamp_types);
    if (n_tstamp_types > 0){
        int err = pcap_set_tstamp_type(p, best_timestamp_type_available);
        if (err) {
            fprintf(stderr, "Error when setting tstamp_type %d\n", best_timestamp_type_available);
        }
    }

    int precision = pcap_get_tstamp_precision(p);
    if (precision == PCAP_TSTAMP_PRECISION_MICRO) {
        // try to set nanosecond resolution
        pcap_set_tstamp_precision(p, PCAP_TSTAMP_PRECISION_NANO);
    }

    if(pcap_set_immediate_mode(p, 1) == 0){
        std::cout << "Enabled immediate mode." << std::endl;
    }else{
        std::cerr << "Unable to set immediate moode!." << std::endl;
    }
    /* end of custom code */
    
    status = pcap_set_snaplen(p, snaplen);
    if (status < 0)
        goto fail;
    status = pcap_set_promisc(p, promisc);
    if (status < 0)
        goto fail;
    status = pcap_set_timeout(p, to_ms);
    if (status < 0)
        goto fail;

    status = pcap_activate(p);
    if (status < 0)
        goto fail;
    return (p);
fail:
    fprintf(stderr, "custom_pcap_open_live: error!\n");
    pcap_close(p);
    return (NULL);
}

PcapWrapper::PcapWrapper() {
    device = pcap_lookupdev(errbuf);
    if (device == NULL) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        throw;
    }

    init();
}

PcapWrapper::PcapWrapper(const char *device) {
    this->device = device;

    init();
}

void PcapWrapper::init() {
    printf("Device: %s\n", device);

    handle = custom_pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\nThis is probably a permission issue.", device, errbuf);
        throw;
    }

    int precision = pcap_get_tstamp_precision(handle);
    switch (precision) {
        case PCAP_TSTAMP_PRECISION_MICRO:
            printf("Using microsecond resolution\n");
            break;
            
        case PCAP_TSTAMP_PRECISION_NANO:
            printf("Using nanosecond resolution\n");
            break;
    }
    linktype = (PcapLoopCallback::LinkType)pcap_datalink(handle);
    usrdata.linktype = linktype;
    usrdata.handle = handle;
    usrdata.tstamp_precision = precision;
    printf("Initialized device for listening\n");
}

void PcapWrapper::setFilter(const char* remote_host, uint16_t remote_port){
    std::string filter = "((src port "+std::to_string(remote_port)+" and src "+remote_host + ") or (dst port "+std::to_string(remote_port)+" and dst "+remote_host+"))";
    std::cout << filter << std::endl;
    
    bpf_u_int32 mask;        /* The netmask of our sniffing device */
    bpf_u_int32 net;        /* The IP of our sniffing device */
    
    if (pcap_lookupnet(device, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", device);
        net = 0;
        mask = 0;
    }
    
    if (pcap_compile(handle, &fp, filter.c_str(), 0, net) == -1) {
        fprintf(stderr, "Couldn't compile filter %s: %s\n", filter.c_str(), pcap_geterr(handle));
        throw;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter.c_str(), pcap_geterr(handle));
        throw;
    }
    usrdata.remote_host = remote_host;
    usrdata.remote_port = remote_port;
}

struct timeval PcapWrapper::timingForPacket(const void* buf, size_t buflen, PcapLoopCallback::PacketDirection direction) {
    bool found_first = false;
    const struct timeval* match = nullptr;


    while (!found_first) {

        if (usrdata.active_buffer_producer == usrdata.active_buffer_consumer && usrdata.shared_buffer_index_producer == usrdata.shared_buffer_index_consumer) {
            // no new packets
            std::this_thread::yield();
            continue;
        }
        if (usrdata.active_buffer_producer != usrdata.active_buffer_consumer && usrdata.shared_buffer_index_consumer == PcapLoopCallback::SHARED_BUFFER_SIZE) {
            // we are at the end of the current buffer
            // switch to the other buffer
            std::this_thread::yield();
            usrdata.active_buffer_consumer = !usrdata.active_buffer_consumer;
            usrdata.shared_buffer_index_consumer = 0;
            continue;
        }

        PcapLoopCallback::PacketInfo* current_buffer = usrdata.active_buffer_consumer ? usrdata.shared_buffer_b : usrdata.shared_buffer_a;
        PcapLoopCallback::PacketInfo& candidate = current_buffer[usrdata.shared_buffer_index_consumer];


        /* test if we found a match */
        if (
                direction == candidate.direction &&
                buflen    == candidate.payload_size &&
                memcmp(buf, candidate.payload, buflen) == 0
                )
        {
            found_first = true;
            match = &candidate.timing;
        }

        usrdata.shared_buffer_index_consumer++;
    }

    return {match->tv_sec, match->tv_usec};
}

void loop(PcapLoopCallback::UserData * usrdata) {
    printf("%s\n", "Starting loop!");
    pcap_loop(usrdata->handle, -1, PcapLoopCallback::handlePacket, (u_char*)usrdata);
    printf("%s\n", "Stopping loop!");
}

void PcapWrapper::startLoop() {
    loop_thread = new std::thread(loop, &usrdata);
}

void PcapWrapper::stopLoop() {
    using namespace std::chrono_literals;
    auto thread_loop_thread_handle = loop_thread->native_handle();
    pcap_breakloop(handle);
    usrdata.stop_loop = true;
    //loop_thread->join();
    //std::this_thread::sleep_for(2s);
    // TODO: Terminate thread after timeout if it is blocking
}

int PcapWrapper::getPrecision() {
    return usrdata.tstamp_precision;
}
