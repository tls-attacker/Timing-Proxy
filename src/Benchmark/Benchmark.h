#ifndef Benchmark_h
#define Benchmark_h

#include "../TimeSources/TimeSources.h"

class Benchmark{
    TimeSources::time_source_func source;
    uint64_t iterations;
    
public:
    Benchmark(TimeSources::time_source_func source, uint64_t iterations);
    uint64_t overhead();
};

#endif // Benchmark_h
