/* schedule.cpp */

#include "pq.h"

void fifo() {
	debug("FIFO");
}

void rdrb() {
	debug("RDRB");
}

void mlfq() {
	debug("MLFQ");
}


/* This schedule function is called when a running job dies or when the time
 * slice period ends. Then according to the policy specified, it will schedule
 * jobs accordingly by calling the respective function */
void schedule(uint64_t time_slice) {
	
	/* compare time to last call time of scheduler */
	if(getCurrentTime() - time_slice < s_struct->last_called) return;

	/* call proper scheduling function according to scheduling policy */
	switch(s_struct->policy) {
		case FIFO:
			fifo();
			break;
		case RDRB:
			rdrb();
			break;
		case MLFQ:
			mlfq();
			break;
	}
}
