#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cpuid.h>
#include <ctime>
//#include <Kernel/i386/cpuid.h>

#include "TimeSources/TimeSources.h"
#include "pcapMeasure/TimePackets.h"
#include "TimingSocket/TimingSocket.h"
#include "Benchmark/Benchmark.h"

int main() {
    
    TimeSources::cpu_features features = TimeSources::get_cpu_features();
    if (features.constant_tsc) {
        std::cout << "Has constant_tsc!" << std::endl;
    }else{
        std::cout << "Doesn't have constant_tsc!" << std::endl;
    }
    printf("Processor baseclock: %llu Hz\n", features.processor_base_clock_hz);
    
    Benchmark os_bench(TimeSources::osTime, 1000);
    std::cout << "Overhead of os time   : " << os_bench.overhead() << std::endl;
    
    Benchmark cpu_bench(TimeSources::timestampCounter, 1000);
    std::cout << "Overhead of cpu time  : " << cpu_bench.overhead() << std::endl;
    
    Benchmark clock_bench(TimeSources::clock, 1000);
    std::cout << "Overhead of clock time: " << clock_bench.overhead() << std::endl;
    
    unsigned int level = 0;
    unsigned int eax = 0;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    __get_cpuid(level, &eax, &ebx, &ecx, &edx);
    
    TimingSocket ts;
    ts.connect("127.0.0.1", 8080);
    std::string msg = "Hello Server\n";
    uint64_t timing = ts.writeAndTimeResponse((void*)msg.c_str(), msg.size()+1);
    void* buf = malloc(1024);
    size_t nread = ts.read(buf, 1023);
    ((uint8_t*)buf)[nread] = '\0';
    printf("%s\n", buf);
    free(buf);
    std::cout << timing << std::endl;
    ts.close();
    

    return 0;
}
