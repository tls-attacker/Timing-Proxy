//
//  TimeSources.h
//
//
//  Created by Malte Poll on 18.09.18.
//

#ifndef TimeSources_h
#define TimeSources_h

#include <cstdint>

namespace TimeSources {
    struct cpu_features{
        uint64_t processor_base_clock_hz;
        bool invariant_tsc;
        bool constant_tsc;
        bool rdtscp;
    };
    
    typedef uint64_t (*time_source_func)();
    
    struct cpu_features get_cpu_features();
    time_source_func best_timesource(struct TimeSources::cpu_features features);
    uint64_t timestampCounter();
    uint64_t osTime();
    uint64_t clock();
}

#endif /* TimeSources.h */
