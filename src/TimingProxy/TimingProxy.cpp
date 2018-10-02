#include "TimingProxy.h"
#include <vector>
#include <iostream>
#include <sstream>

void TimingProxy::tryForwardInput() {
    char buf[1024];
    ssize_t size_read = proxy_input.read(buf, 1024, false);
    if (size_read > 0) {
        std::string timing = std::to_string(proxy_output.writeAndTimeResponse(buf, size_read))+"\n";
        control.write(timing.c_str(), timing.length());
    }
}

void TimingProxy::tryForwardOutput() {
    char buf[1024];
    ssize_t size_read = proxy_output.read(buf, 1024, false);
    if (size_read > 0) {
        proxy_input.write(buf, size_read);
    }
}

void TimingProxy::run() {
    control.listen();
    proxy_input.listen();
    control.accept();
    proxy_input.accept();
    getProxyTarget();
    proxy_output.connect(connect_host, connect_port);
    while (true) {
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
