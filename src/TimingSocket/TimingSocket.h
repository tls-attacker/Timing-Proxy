#ifndef TIMING_TIMINGSOCKETS_H
#define TIMING_TIMINGSOCKETS_H

#include <string>
#include "../TimeSources/TimeSources.h"

enum sockState{
    SOCKSTATE_UNINITIALIZED,
    SOCKSTATE_ESTABLISHED,
    SOCKSTATE_CLOSED,
};

class TimingSocket{
    sockState state = SOCKSTATE_UNINITIALIZED;
    int sock;
    TimeSources::cpu_features cpu_features;
    uint64_t (*best_timesource)() = nullptr;
    
    
public:
    TimingSocket();
    void connect(std::string host, int port);
    void close();
    void write(const void* data, size_t size);
    uint64_t writeAndTimeResponse(const void *data, size_t size);
    ssize_t read(void *buf, size_t size, bool blocking = true);
    
};

#endif // TIMING_TIMINGSOCKETS_H
