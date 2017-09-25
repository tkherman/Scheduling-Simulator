/* schedule.cpp */

#include "pq.h"
#include <unistd.h>
#include <cstring>


pid_t run_process(Process * next) {
	/* fork a new process */
	pid_t id = fork();
	
	if(id > 0) {


		return id;

	} else if (id == 0) { //child process
		/* manually create non-const c string */
		char cmd[next->command.size()+1];
		size_t length = next->command.copy(cmd, next->command.size());
		cmd[length] = '\0';

		/* parse command string */
		char * arglist[20];
		char * arg = strtok(cmd, " ");
		int k=0;
		while(arg != NULL) {
			arglist[k++] = arg;
			arg = strtok(NULL, " ");
			debug(arg);
		}
		arglist[k] = NULL;
		

		/* execute the command */
		server_log("about to exec");
		cout << arglist[0] << endl;
		if(execvp(arglist[0], arglist) < 0) {
			server_log("exec failed");
			//TODO check what do do on error (i.e. with deallocation)
			_exit(EXIT_FAILURE);
		}
		debug("here");

		
	} else { // fork failed
		server_log("Fork failed");
        return -1;
	}

    return -1;

}

void fifo() {
	debug("entered fifo scheduler");
	
	while(int(s_struct->running_jobs.size()) < s_struct->ncpu) {
		
		if(s_struct->waiting_jobs.empty())
			break;

		/* Access the struct Process for the next job to run */
		Process * next = s_struct->waiting_jobs.back();
		s_struct->waiting_jobs.pop_back();
		
        /* Create a new process */
		pid_t new_pid = run_process(next);

		/* If new_pid is -1, handle error. Else, update Process struct */
        if (new_pid == -1) {
            continue;
        }

        next->state = "running";
		next->pid = new_pid;
        next->start = getCurrentTime();
        // get usage

		s_struct->running_jobs.push_front(next);
	}
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
