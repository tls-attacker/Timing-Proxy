//
//  CPUTiming.hpp
//  timing
//
//  Created by Malte Poll on 18.09.18.
//

#ifndef CPUTiming_hpp
#define CPUTiming_hpp

#include <cstdint>

namespace TimeSources {
    struct cpu_features{
        uint64_t processor_base_clock_hz;
        bool invariant_tsc;
        bool constant_tsc;
        bool rdtscp;
    };
    
    struct cpu_features get_cpu_features();
    uint64_t (*best_timesource())();
    uint64_t timestampCounter();
    uint64_t osTime();
}

#endif /* CPUTiming_hpp */
