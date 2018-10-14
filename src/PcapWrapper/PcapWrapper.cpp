//
// Created by Malte Poll on 18.09.18.
//

#include "PcapWrapper.h"

#include <netinet/in.h>
#include <string>
#include <iostream>


/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN    6

/* Ethernet header */
struct sniff_ethernet {
    u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
    u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
    u_short ether_type; /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
    u_char ip_vhl;        /* version << 4 | header length >> 2 */
    u_char ip_tos;        /* type of service */
    u_short ip_len;        /* total length */
    u_short ip_id;        /* identification */
    u_short ip_off;        /* fragment offset field */
#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff    /* mask for fragmenting bits */
    u_char ip_ttl;        /* time to live */
    u_char ip_p;        /* protocol */
    u_short ip_sum;        /* checksum */
    struct in_addr ip_src,ip_dst; /* source and dest address */
};
#define IP_HL(ip)        (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)        (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;    /* source port */
    u_short th_dport;    /* destination port */
    tcp_seq th_seq;        /* sequence number */
    tcp_seq th_ack;        /* acknowledgement number */
    u_char th_offx2;    /* data offset, rsvd */
#define TH_OFF(th)    (((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;        /* window */
    u_short th_sum;        /* checksum */
    u_short th_urp;        /* urgent pointer */
};
/* ethernet headers are always exactly 14 bytes */
#define SIZE_ETHERNET 14

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
    std::string filter = "tcp port "+std::to_string(remote_port)+" and (src "+remote_host + " or dst "+remote_host+")";
    std::cout << filter << std::endl;
    
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

uint64_t PcapWrapper::timingForPacket(const void* buf, size_t buflen) {
    bool foundPacket = false;
    struct pcap_pkthdr header;
    const u_char *packet = nullptr;
    const struct sniff_ethernet *ethernet; /* The ethernet header */
    const struct sniff_ip *ip; /* The IP header */
    const struct sniff_tcp *tcp; /* The TCP header */
    const char *payload; /* Packet payload */
    u_int size_ip;
    u_int size_tcp;
    while (!foundPacket) {
        while (!packet) {
            packet = pcap_next(handle, &header);
        }
        ethernet = (struct sniff_ethernet*)(packet);
        ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
        size_ip = IP_HL(ip)*4;
        if (size_ip < 20) {
            printf("   * Invalid IP header length: %u bytes\n", size_ip);
            goto loop_err;
        }
        tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
        size_tcp = TH_OFF(tcp)*4;
        if (size_tcp < 20) {
            printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
            goto loop_err;
        }
        payload = (char*)(packet + SIZE_ETHERNET + size_ip + size_tcp);
        printf("Got a tcp payload\n");
        printf("%s\n", packet);
        
    loop_err:
        packet = nullptr;
    }
    return 0;
    

}

void loop() {
    printf("%s\n", "Starting loop!");
}

void PcapWrapper::startLoop() {
    loop_thread = new std::thread(loop);
}

void PcapWrapper::stopLoop() {
    
}
