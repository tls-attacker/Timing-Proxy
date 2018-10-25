#ifndef Helper_H
#define Helper_H

#include <stdio.h>
#include <chrono>
#include <thread>
#include <random>

namespace helper {
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 1);

	void log(const char* msg) {
		fprintf(stderr, "%s\n", msg);
	}

    void initRandomSeed() {
        srand(time(0));
    }

    bool randomBool() {
        return rand() % 2 == 0;
    }

    double getRandomPercentage() {
        return dis(e);
    }

	void passive_sleep(uint64_t nanoseconds) {
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(nanoseconds*1ns);
	}

	void active_sleep(uint64_t nanoseconds) {
        for (uint64_t i = 0; i < nanoseconds; ++i) { }
	}
}

#endif // Helper_h
