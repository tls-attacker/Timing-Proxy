#include "TimingProxy.h"
#include "../Socket/ClientSocket/TimingSocket/PCAPTimingSocket/PCAPTimingSocket.h"
#include "../Socket/ClientSocket/TimingSocket/KernelTimingSocket/KernelTimingSocket.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <tuple>
#include <boost/endian/conversion.hpp>


void TimingProxy::tryForwardInput() {
    char buf[1024];
    ssize_t size_read = proxy_input.read(buf, 1024, false);
    if (size_read > 0) {
        proxy_output->write(buf, size_read);
    }
}

void TimingProxy::tryForwardOutput() {
    char buf[1024];
    ssize_t size_read = proxy_output->read(buf, 1024, false);

    if (size_read > 0) {
        proxy_input.write(buf, size_read);

        //std::string timing = std::to_string(proxy_output->getLastMeasurement())+"\n";
        //control.write(timing.c_str(), timing.length());
        uint64_t network_byte_order_tstamp = boost::endian::native_to_big(proxy_output->getLastMeasurement());
        control.write(&network_byte_order_tstamp, sizeof(uint64_t));
    }
}

void TimingProxy::run() {
    control.listen();
    proxy_input.listen();
    control.accept();
    proxy_input.accept();
    getProxyTarget();
    proxy_output->connect(connect_host, connect_port);

    while (true) {
        std::cout << "Still alive!" << std::endl;
        tryForwardInput();
        tryForwardOutput();
    }
}

std::tuple<std::string, std::string> TimingProxy::getline(std::string prepend) {
    std::string str = prepend;
    std::string head;
    std::string tail;
    char buf[1025];
    size_t pos = std::string::npos;
    while (pos == std::string::npos) {
        size_t len_read = control.read(buf, 1024);
        buf[len_read] = 0;
        std::vector<std::string> strings;
        str += std::string(buf);
        pos = str.find("\n");
        if (pos!=std::string::npos) {
            head = str.substr(0, pos);
            tail = str.substr(pos+1);
        }
    }
    return std::make_tuple(head, tail);
}

void TimingProxy::getProxyTarget() {
    std::tuple<std::string, std::string> result = getline("");
    connect_host = std::get<0>(result);
    std::cout << connect_host << "\n";
    result = getline(std::get<1>(result));
    connect_port = std::stoi(std::get<0>(result));
    std::cout << connect_port << "\n";
}

void TimingProxy::setInterface(std::string interface) {
    if (measurement_technique == Socket::TimingSocket::KindOfSocket::Kernel) {
        dynamic_cast<Socket::KernelTimingSocket*>(proxy_output.get())->enableHardwareTimestampingForDevice(interface);
    }else if(measurement_technique == Socket::TimingSocket::KindOfSocket::PCAP) {
        dynamic_cast<Socket::PCAPTimingSocket*>(proxy_output.get())->initPcap(interface);
    }
}
