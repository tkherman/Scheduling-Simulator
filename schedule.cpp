/* schedule.cpp */

#include "pq.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <signal.h>
#include <fstream>

pid_t run_process(Process * next) {
	
    /* Fork a new process */
	pid_t id = fork();
	
	if(id > 0) { // Parent

		return id;
        
	} else if (id == 0) { // Child

		/* Manually create non-const c string */
		char cmd[next->command.size()+1];
		size_t length = next->command.copy(cmd, next->command.size());
		cmd[length] = '\0';

		/* Parse command string into char*[] */
		char * arglist[20];
		char * arg = strtok(cmd, " ");
		int k=0;
		while(arg != NULL) {
			arglist[k++] = arg;
			arg = strtok(NULL, " ");
		}
		arglist[k] = NULL;
	    
        /* Redirect output */
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 1);

		/* Execute the command */
		if(execvp(arglist[0], arglist) < 0) {
			server_log("Error: exec failed");
			return -1;
		}

		
	} else { // fork failed
		server_log("Error: fork failed");
        return -1;
	}

    return -1;

}

void fifo() {
	/* Move jobs to running queue if there's free cpu */
    while(int(s_struct->running_jobs.size()) < s_struct->ncpu) {
		
		if(s_struct->waiting_jobs.empty())
			break;

		/* Access the struct Process for the next job to run */
		Process * next = s_struct->waiting_jobs.back();
		s_struct->waiting_jobs.pop_back();
		
        /* Create a new process */
		pid_t new_pid = run_process(next);

        server_log("Started process " << new_pid << " " << next->command);

		/* If new_pid is -1, delete the process struct
         * Else, update Process struct */
        if (new_pid == -1) {
            delete next;
            continue;
        }

        /* Update process' stat */
        Process_Stat ps = get_process_stat(new_pid);

        next->state = ps.state;
		next->pid = new_pid;
        next->start = getCurrentTime();
        next->cpu_usage = ps.cpu_usage;

		s_struct->running_jobs.push_front(next);
	}

    /* Update process stat for jobs in running */
    for (auto &p : s_struct->running_jobs) {
        Process_Stat ps = get_process_stat(p->pid);
        p->cpu_usage = ps.cpu_usage;
        p->state = ps.state;
    }

}

void rdrb() {
    
    /* Move a process from running queue to waiting queue */
	while(s_struct->running_jobs.size()) {
		Process * to_stop = s_struct->running_jobs.back();
		s_struct->waiting_jobs.push_front(to_stop);
		s_struct->running_jobs.pop_back();
		
		/* Signal process to stop */
		kill(to_stop->pid, SIGSTOP);
	}
	
    /* Move process from waiting queue to running queue */
	while(int(s_struct->running_jobs.size()) < s_struct->ncpu) {
		
		if(s_struct->waiting_jobs.empty())
			break;

		/* Get the next job to move */
		Process * next = s_struct->waiting_jobs.back();
		s_struct->waiting_jobs.pop_back();
	    
        /* Start new process */
		if(next->pid == 0) {
			
            pid_t npid = run_process(next);
			
            if (npid < 0) { 
				delete next;
                continue;
			}

            server_log("Started process " << npid << " " << next->command);

            /* Update process stat */
            Process_Stat ps = get_process_stat(npid);

			next->state = ps.state;
            next->pid = npid;
        	next->start = getCurrentTime();
            next->cpu_usage = ps.cpu_usage;
            next->user_time = ps.user_time;
			
            s_struct->running_jobs.push_front(next);
        
        /* Resume old process */
		} else {
            
            /* Update process stat */
            Process_Stat ps = get_process_stat(next->pid);

			next->state = ps.state;
            next->pid = next->pid;
            next->cpu_usage = ps.cpu_usage;
			
            s_struct->running_jobs.push_front(next);
			kill(next->pid, SIGCONT);
		}
	}
    

    /* Update process stat for jobs in running */
    for (auto &p : s_struct->running_jobs) {
        Process_Stat ps = get_process_stat(p->pid);
        p->cpu_usage = ps.cpu_usage;
        p->state = ps.state;
        p->user_time = ps.user_time;
    }
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
