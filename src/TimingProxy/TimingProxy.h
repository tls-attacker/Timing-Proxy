#ifndef TimingProxy_h
#define TimingProxy_h

#include <string>
#include "../Socket/ServerSocket/ServerSocket.h"
#include "../Socket/ClientSocket/TimingSocket/TimingSocket.h"

class TimingProxy {
    Socket::ServerSocket proxy_input;
    Socket::ServerSocket control;
    std::unique_ptr<Socket::TimingSocket> proxy_output;
    std::string connect_host;
    int connect_port;
    std::tuple<std::string, std::string> getline(std::string prepend);
    void getProxyTarget();
    void tryForwardInput();
    void tryForwardOutput();
    void handleClient();
    Socket::TimingSocket::KindOfSocket measurement_technique;
    
public:
    TimingProxy(int listen_port, int control_port, Socket::TimingSocket::KindOfSocket measurement_technique) :  proxy_output(Socket::TimingSocket::createTimingSocket(measurement_technique)) {
        control.bind(control_port);
        proxy_input.bind(listen_port);
        this->measurement_technique = measurement_technique;
    };
    void run();
    void setInterface(std::string interface);
};

#endif // TimingProxy_h
