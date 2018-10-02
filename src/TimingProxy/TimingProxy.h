#ifndef TimingProxy_h
#define TimingProxy_h

#include <string>
#include "../ServerSocket/ServerSocket.h"
#include "../TimingSocket/TimingSocket.h"

class TimingProxy {
    ServerSocket proxy_input;
    ServerSocket control;
    TimingSocket proxy_output;
    std::string connect_host;
    int connect_port;
    std::tuple<std::string, std::string> getline(std::string prepend);
    void getProxyTarget();
    void tryForwardInput();
    void tryForwardOutput();
    
public:
    TimingProxy(int listen_port, int control_port) {
        control.bind(control_port);
        proxy_input.bind(listen_port);
    };
    void run();
};

#endif // TimingProxy_h
