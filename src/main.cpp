#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cpuid.h>
#include <ctime>
//#include <Kernel/i386/cpuid.h>
#include "TimingSocket/TimingSocket.h"
#include "TimingProxy/TimingProxy.h"

int main() {
    TimingProxy timingProxy(4444, 5555, TimingSocket::KindOfSocket::Kernel);
    timingProxy.run();
    return 0;
}
