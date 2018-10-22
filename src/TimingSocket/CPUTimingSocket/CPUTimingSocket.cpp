//
// Created by malte on 21.10.18.
//

#include <sys/socket.h>
#include "CPUTimingSocket.h"

CPUTimingSocket::CPUTimingSocket() {
    init();
}

void CPUTimingSocket::init() {
    cpu_features = TimeSources::get_cpu_features();
    best_timesource = TimeSources::best_timesource(cpu_features);
}


uint64_t CPUTimingSocket::writeAndTimeResponse(const void *data, size_t size) {
    uint8_t tmp_buf;
    uint64_t timing;
    write(data, size);
    /*start timing*/
    timing = best_timesource();
    ::recv(sock, &tmp_buf, 1, MSG_PEEK);
    /*end timing*/
    timing = best_timesource() - timing;
    return timing;
}