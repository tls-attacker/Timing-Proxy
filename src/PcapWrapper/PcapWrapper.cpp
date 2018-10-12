//
// Created by Malte Poll on 18.09.18.
//

#include "PcapWrapper.h"

#include <string>

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
    
    pcap_set_tstamp_type(p, best_timestamp_type_available);
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
        fprintf(stderr, "Couldn't open device %s: %s\n", device, errbuf);
        throw;
    }
    int precision = pcap_get_tstamp_precision(handle);
    if (precision == PCAP_TSTAMP_PRECISION_MICRO) {
        // try to set nanosecond resolution
        pcap_set_tstamp_precision(handle, PCAP_TSTAMP_PRECISION_NANO);
    }
    precision = pcap_get_tstamp_precision(handle);
    switch (precision) {
        case PCAP_TSTAMP_PRECISION_MICRO:
            printf("Using microsecond resolution\n");
            break;
            
        case PCAP_TSTAMP_PRECISION_NANO:
            printf("Using nanosecond resolution\n");
            break;
    }
    
    printf("Initialized device for listening\n");
}

void PcapWrapper::setFilter(const char* remote_host, int remote_port){
    std::string filter = "tcp port "+std::to_string(remote_port)+" and src "+remote_host;
    
    bpf_u_int32 mask;        /* The netmask of our sniffing device */
    bpf_u_int32 net;        /* The IP of our sniffing device */
    
    if (pcap_lookupnet(device, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", device);
        net = 0;
        mask = 0;
    }
    
    if (pcap_compile(handle, &fp, filter.c_str(), 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter.c_str(), pcap_geterr(handle));
        throw;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter.c_str(), pcap_geterr(handle));
        throw;
    }
}
