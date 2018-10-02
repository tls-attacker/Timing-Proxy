#ifndef Helper_H
#define Helper_H

#include <stdio.h>

namespace helper {
	void log(const char* msg) {
		fprintf(stderr, "%s\n", msg);
	}
}

#endif // Helper_h
