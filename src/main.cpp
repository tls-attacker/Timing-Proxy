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
    size_t framesize = 66;
    char tcp_frame[67] = "\x00\x90\x4c\x0d\xf4\x3e\xc8\x0e\x14\xaf\xf6\x7f\x08\x00\x45\x00" \
"\x00\x34\x35\x33\x40\x00\x37\x06\x7b\x71\x40\x9e\xdf\x9d\xc0\xa8" \
"\xb2\x3b\x01\xbb\x99\xba\x05\xe8\x64\x98\xb6\xf2\xd5\x3f\x80\x10" \
"\x00\x41\x4f\x78\x00\x00\x01\x01\x08\x0a\x0d\x6c\x66\x3a\x1d\x9d" \
"\x70\x78";

    void* tcp_payload;
    size_t tcp_payload_size;
    PacketParser::decodeUntil(PacketParser::Layer::tcp, tcp_frame, framesize, &tcp_payload, &tcp_payload_size);

    PcapWrapper pc = PcapWrapper("lo");
    pc.setFilter("127.0.0.1", 8888);
    char buf[10] = "test";
    pc.timingForPacket((void*)buf, 10);
    return 0;
}
