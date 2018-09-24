#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cpuid.h>
//#include <Kernel/i386/cpuid.h>

#include "TimeSources/CPUTiming.h"
#include "pcapMeasure/TimePackets.h"
#include "TimingSocket/TimingSocket.h"

int main() {
    
    TimeSources::cpu_features features = TimeSources::get_cpu_features();
    if (features.constant_tsc) {
        std::cout << "Has constant_tsc!" << std::endl;
    }else{
        std::cout << "Doesn't have constant_tsc!" << std::endl;
    }
    
    
    uint64_t os_time = TimeSources::osTime();
    uint64_t cpu_time = TimeSources::timestampCounter();
    sleep(1);
    os_time = TimeSources::osTime() - os_time;
    cpu_time = TimeSources::timestampCounter() - cpu_time;
    std::cout << "OS Time: " << os_time << std::endl;
    std::cout << "CPU Time: " << cpu_time << std::endl;
    
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
