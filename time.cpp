/* time.cpp */
/* Includes function to return current time in microseconds */

#include "pq.h"
#include <sys/time.h>

uint64_t getCurrentTime() {
	/* get current time in microseconds */
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t curr_time = 1000000 * tv.tv_sec + tv.tv_usec;
	return curr_time;
}
