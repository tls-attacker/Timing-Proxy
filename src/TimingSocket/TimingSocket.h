#ifndef TIMING_TIMINGSOCKETS_H
#define TIMING_TIMINGSOCKETS_H

#include <string>
#include <memory>

class TimingSocket{
    enum sockState{
        SOCKSTATE_UNINITIALIZED,
        SOCKSTATE_ESTABLISHED,
        SOCKSTATE_CLOSED,
    };

    sockState state = SOCKSTATE_UNINITIALIZED;
    virtual void init() = 0;

protected:
    int sock;
    std::string host;
    int port;
    
public:
    enum KindOfSocket {
        CPU,
        Kernel,
        PCAP
    };
    virtual ~TimingSocket() = default;
    virtual void connect(std::string host, uint16_t port);
    virtual void close();
    virtual ssize_t read(void *buf, size_t size, bool blocking);
    virtual void write(const void* data, size_t size);
    virtual uint64_t getLastMeasurement() = 0;

    static std::unique_ptr<TimingSocket> createTimingSocket(KindOfSocket kind);
};

#endif // TIMING_TIMINGSOCKETS_H
