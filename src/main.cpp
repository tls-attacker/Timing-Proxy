#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cpuid.h>
#include <ctime>
//#include <Kernel/i386/cpuid.h>
#include "TimingSocket/TimingSocket.h"

int main() {
    //TimingProxy tp(4444, 5555);
    //tp.run();
    std::unique_ptr<TimingSocket> ts = TimingSocket::createTimingSocket(TimingSocket::Kernel);
    ts->connect("169.254.71.233", 1337);
    std::cout << ts->writeAndTimeResponse("Lorem Ipsum Dolor site amed\n", 28) << std::endl;
    ts->close();
    return 0;
}
