#include "Benchmark.h"
#include <iostream>

Benchmark::Benchmark(TimeSources::time_source_func source, uint64_t iterations) {
    this->source = source;
    if (iterations == 0) {
        throw;
    }
    this->iterations = iterations;
}

uint64_t Benchmark::overhead() {
    uint64_t result = 0;
    for (auto i = 0; i<iterations; i++) {
        uint64_t time = source();
        time = source() - time;
        result += time;
    }
    return result / iterations;
}
