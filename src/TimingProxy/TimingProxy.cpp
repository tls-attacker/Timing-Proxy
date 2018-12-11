#include "TimingProxy.h"
#include "../Socket/ClientSocket/TimingSocket/PCAPTimingSocket/PCAPTimingSocket.h"
#include "../Socket/ClientSocket/TimingSocket/KernelTimingSocket/KernelTimingSocket.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <tuple>
#include <boost/endian/conversion.hpp>

#define BUFSIZE (2500)


void TimingProxy::tryForwardInput() {
    char buf[BUFSIZE];
    ssize_t size_read = proxy_input.read(buf, BUFSIZE, false);
    if (size_read > 0) {
        proxy_output->write(buf, size_read);
    }else if(size_read == 0){
        /* other side closed connection */
        throw std::runtime_error("Proxy input closed connection");
    }else if(size_read != -1){
        std::cout << "reading from client returned: "<<size_read<<std::endl;
    }
}

void TimingProxy::tryForwardOutput() {
    char buf[BUFSIZE];
    ssize_t size_read = proxy_output->read(buf, BUFSIZE, false);

    if (size_read > 0) {
        proxy_input.write(buf, size_read);

        //std::string timing = std::to_string(proxy_output->getLastMeasurement())+"\n";
        //control.write(timing.c_str(), timing.length());
        uint64_t network_byte_order_tstamp = boost::endian::native_to_big(proxy_output->getLastMeasurement());
        control.write(&network_byte_order_tstamp, sizeof(uint64_t));
    }else if(size_read == 0){
        /* other side closed connection */
        throw std::runtime_error("Proxy output closed connection");
    }else if(size_read != -1){
        std::cout << "reading from server returned: "<<size_read<<std::endl;
    }
}

void TimingProxy::run() {
    control.listen();
    proxy_input.listen();
    while (true) {
        handleClient();
    }
}

void TimingProxy::handleClient() {
    control.accept();
    getProxyTarget();
    proxy_input.accept();
    proxy_output->connect(connect_host, connect_port);
    bool connection_established = true;
    std::cout << "Proxy connection established!\n" << std::endl;

    while (connection_established) {
        try{
            tryForwardInput();
            tryForwardOutput();
        }catch (const std::runtime_error& e) {
            std::cerr << "Terminating proxy connection. Reason: " << e.what() << std::endl;
            connection_established = false;
        }

        if (proxy_input.socketPeerClosed() || control.socketPeerClosed() || proxy_output->socketPeerClosed()) {
            /* If any side closes the connection, shut down */
            connection_established = false;
        }
    }

    /* shut down the connection */
    std::cout << "Proxy connection closed" << std::endl;
    proxy_input.close_client();
    control.close_client();
    proxy_output->close();
}

std::tuple<std::string, std::string> TimingProxy::getline(std::string prepend) {
    std::string str = prepend;
    std::string head;
    std::string tail;
    char buf[1025];
    size_t pos = std::string::npos;
    while (pos == std::string::npos) {
        pos = str.find("\n");
        if (pos!=std::string::npos) {
            head = str.substr(0, pos);
            tail = str.substr(pos+1);
        }else{
            size_t len_read = control.read(buf, 1024);
            buf[len_read] = 0;
            str += std::string(buf);
        }
    }
    return std::make_tuple(head, tail);
}

void TimingProxy::getProxyTarget() {
    std::tuple<std::string, std::string> result = getline("");
    connect_host = std::get<0>(result);
    result = getline(std::get<1>(result));
    connect_port = std::stoi(std::get<0>(result));
    std::cout << "New proxy connection: " << connect_host << ":" << connect_port << "\n";
}

void TimingProxy::setInterface(std::string interface) {
    if (measurement_technique == Socket::TimingSocket::KindOfSocket::Kernel) {
        dynamic_cast<Socket::KernelTimingSocket *>(proxy_output.get())->setDevice(interface);
    }else if(measurement_technique == Socket::TimingSocket::KindOfSocket::PCAP) {
        dynamic_cast<Socket::PCAPTimingSocket*>(proxy_output.get())->initPcap(interface);
    }
}
