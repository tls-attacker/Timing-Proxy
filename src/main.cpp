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

int main() {
    TimingProxy tp(4444, 5555);
    tp.run();
    return 0;
}
