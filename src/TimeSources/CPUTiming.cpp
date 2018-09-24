//
//  CPUTiming.cpp
//  timing
//
//  Created by Malte Poll on 18.09.18.
//

#include "CPUTiming.h"
#include <sys/time.h>
#include <mach/mach_time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

#if defined(__x86_64__) || defined(__i386)
inline uint64_t rdtsc(){
    uint32_t lo,hi;
    __asm__ __volatile__ ("RDTSCP" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif

uint64_t TimeSources::timestampCounter() {
#if defined(__x86_64__) || defined(__i386)
    /* x86 and x86-64 */
    return rdtsc();
#endif
}

#if defined(unix)
inline uint64_t unix_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*(uint64_t)1000000+ts.tv_nsec;
}
#endif

#if defined(__MACH__)
inline uint64_t darwin_time() {
    return mach_absolute_time();
}
#endif

#if defined(BSD)
inline uint64_t bsd_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_PRECISE, &ts);
    return ts.tv_sec*(uint64_t)1000000+ts.tv_nsec;
}
#endif

uint64_t TimeSources::osTime() {
#if defined(__MACH__)
    return darwin_time();
#elif defined(BSD)
    return bsd_time();
#elif defined(unix)
    return unix_time();
#endif
}

#define BIT(n) (1<<n)

static inline void
do_cpuid(uint32_t selector, uint32_t *data)
{
    __asm__ volatile ("cpuid"
                      : "=a" (data[0]),
                      "=b" (data[1]),
                      "=c" (data[2]),
                      "=d" (data[3])
                      : "a"(selector),
                      "b" (0),
                      "c" (0),
                      "d" (0));
}

bool has_rdtscp() {
#if defined(__x86_64__) || defined(__i386)
    // on x86, an invariant tsc is available if the flag CPUID.80000001H:EDX[27] is set.
    uint32_t data[4];
    // query cpuid with eax = 80000001
    do_cpuid(0x80000001, data);
    // test if edx[27] is set
    uint32_t* edx = &data[3];
    return (BIT(27) & *edx) != 0;
#else
    return false;
#endif
}

bool has_invariant_tsc() {
#if defined(__x86_64__) || defined(__i386)
    // on x86, an invariant tsc is available if the flag CPUID.80000007H:EDX[8] is set.
    uint32_t data[4];
    // query cpuid with eax = 0x80000007
    do_cpuid(0x80000007, data);
    // test if edx[8] is set
    uint32_t* edx = &data[3];
    return (BIT(8) & *edx) != 0;
#else
    return false;
#endif
}

bool has_constant_tsc() {
#if defined(__x86_64__) || defined(__i386)
    if (has_invariant_tsc()) {
        // if the cpu has an invariant tsc, it also has a constant tsc
        return true;
    } else {
#if defined(__linux__)
        std::ifstream infile("/proc/cpuinfo");
        std::string fileData(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
        return fileData.find(std::string("constant_tsc")) != std::string::npos;
#else // not linux (no way to check for constant_tsc so assuming false)
        return false;
#endif // defined(__linux__)
    }
    
#else // not x86 or x86_64
    return false;
#endif // defined(__x86_64__) || defined(__i386)
}

struct TimeSources::cpu_features TimeSources::get_cpu_features(){
    struct cpu_features features;
    features.invariant_tsc = has_invariant_tsc();
    features.constant_tsc = has_constant_tsc();
    features.rdtscp = has_rdtscp();
    return features;
}

uint64_t (*TimeSources::best_timesource())(){
    cpu_features features = get_cpu_features();
    if (features.constant_tsc) {
        return timestampCounter;
    }else{
        return osTime;
    }
    
}

