//
// Created by malte on 21.10.18.
//

#include <sys/socket.h>
#include <iostream>
#include "CPUTimingSocket.h"

Socket::CPUTimingSocket::CPUTimingSocket() {
    init();
}

void Socket::CPUTimingSocket::init() {
    cpu_features = TimeSources::get_cpu_features();
    best_timesource = TimeSources::best_timesource(cpu_features);
}

void Socket::CPUTimingSocket::write(const void *data, size_t size) {
    uint8_t tmp_buf;
    TimingSocket::write(data, size);
    /*start timing*/
    write_tstamp = best_timesource();
    if (takeTimeOnWrite) {
        ::recv(sock, &tmp_buf, 1, MSG_PEEK);
        /*end timing*/
        read_tstamp = best_timesource();
    }
    std::cout << "Set tx_tstamp: "<<write_tstamp<<std::endl;
    if (takeTimeOnWrite) {
        std::cout << "Set rx_tstamp: "<<read_tstamp<<std::endl;
    }
}

ssize_t Socket::CPUTimingSocket::read(void *buf, size_t size, bool blocking) {
    auto bytes_read = TimingSocket::read(buf, size, blocking);
    if (takeTimeOnWrite) {
        return bytes_read;
    }else{
        read_tstamp = best_timesource();
        std::cout << "Set rx_tstamp: "<<read_tstamp<<std::endl;
        return bytes_read;
    }
}

uint64_t Socket::CPUTimingSocket::getLastMeasurement() {
    std::cout << "diff: "<<read_tstamp-write_tstamp<<std::endl;
    return read_tstamp-write_tstamp;
}

