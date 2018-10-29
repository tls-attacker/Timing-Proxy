#ifndef TimingProxy_h
#define TimingProxy_h

#include <string>
#include "../ServerSocket/ServerSocket.h"
#include "../TimingSocket/TimingSocket.h"

class TimingProxy {
    ServerSocket proxy_input;
    ServerSocket control;
    std::unique_ptr<TimingSocket> proxy_output;
    std::string connect_host;
    int connect_port;
    std::tuple<std::string, std::string> getline(std::string prepend);
    void getProxyTarget();
    void tryForwardInput();
    void tryForwardOutput();
    TimingSocket::KindOfSocket measurement_technique;
    
public:
    TimingProxy(int listen_port, int control_port, TimingSocket::KindOfSocket measurement_technique) :  proxy_output(TimingSocket::createTimingSocket(measurement_technique)) {
        control.bind(control_port);
        proxy_input.bind(listen_port);
        this->measurement_technique = measurement_technique;
    };
    void run();
    void setInterface(std::string interface);
};

#endif // TimingProxy_h
