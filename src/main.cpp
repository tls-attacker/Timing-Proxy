#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cpuid.h>
#include <ctime>
//#include <Kernel/i386/cpuid.h>

#include "TimeSources/TimeSources.h"
#include "PcapWrapper/PcapWrapper.h"
#include "TimingSocket/TimingSocket.h"
#include "Benchmark/Benchmark.h"
#include "TimingProxy/TimingProxy.h"
#include "PacketParser/PacketParser.h"

int main() {
    //TimingProxy tp(4444, 5555);
    //tp.run();

    PcapWrapper pc = PcapWrapper("lo");
    pc.setFilter("127.0.0.1", 8888);
    pc.startLoop();

    char buf[10] = "test\n";
    uint64_t timing = pc.timingForPacket((void*)buf, 5);
    std::cout << "Timing for packet: " << timing << "ns" <<std::endl;
    buf[0] = 'b';
    timing = pc.timingForPacket((void*)buf, 5, PcapLoopCallback::PacketDirection::SOURCE_REMOTE);
    std::cout << "Timing for packet: " << timing << "ns" <<std::endl;
    pc.stopLoop();
    return 0;
}
