/* schedule.cpp */

#include "pq.h"
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <fstream>

float get_cpu_usage(pid_t pid) {
    
    /* Get system uptime */
    float uptime;

    string system_file = "/proc/uptime";
    ifstream system_ifs(system_file, ifstream::in);
    
    if (!system_ifs.good()) return 0;

    system_ifs >> uptime;
    system_ifs.close();

    /* Open stat file */
    float utime, stime, starttime;

    string filename = "/proc/" + to_string((int) pid) + "/stat";
    debug(filename);
    ifstream stat_ifs(filename, ifstream::in);

    if (!stat_ifs.good()) {debug("hi");return 0;}
    
    string temp;
    for (int i = 1; i <= 22; i++) {
        stat_ifs >> temp;
        cout << temp;
        if (i == 14) utime = stof(temp);
        else if (i == 15) stime = stof(temp);
        else if (i == 22) starttime = stof(temp);
    }
    
    debug(utime << "," << stime << "," << starttime);

    stat_ifs.close();
    

    /* Calculate CPU usage */
    float hertz = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime;
    float seconds = uptime - (starttime / hertz);

    return 100 * ((total_time/hertz) / seconds);
}

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
		

		/* Execute the command */
		server_log("about to exec");
		cout << arglist[0] << endl;
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

		/* If new_pid is -1, delete the process struct
         * Else, update Process struct */
        if (new_pid == -1) {
            delete next;
            continue;
        }

        next->state = "running";
		next->pid = new_pid;
        next->start = getCurrentTime();
        next->cpu_usage = get_cpu_usage(new_pid);

		s_struct->running_jobs.push_front(next);
	}

    /* Update usage for jobs in running */
    for (auto &p : s_struct->running_jobs) {
        p->cpu_usage = get_cpu_usage(p->pid);
    }

}

void rdrb() {

	while(s_struct->running_jobs.size()) {
		Process * to_stop = s_struct->running_jobs.back();
		s_struct->waiting_jobs.push_front(to_stop);
		s_struct->running_jobs.pop_back();
		
		/* actually stop the process */
		kill(to_stop->pid, SIGSTOP);
	}
	
	while(int(s_struct->running_jobs.size()) < s_struct->ncpu) {
		
		if(s_struct->waiting_jobs.empty())
			break;

		/* get next job to add */
		Process * next = s_struct->waiting_jobs.back();
		s_struct->waiting_jobs.pop_back();
		
		if(next->pid == 0) {
			pid_t npid = run_process(next);
			if (npid < 0) { 
				continue;
				//TODO handle error
			}
			next->pid = npid;
        	next->start = getCurrentTime();
			s_struct->running_jobs.push_front(next);
			//TODO update info about process
		} else {
			kill(next->pid, SIGCONT);
			debug("resumed");
			s_struct->running_jobs.push_front(next);
		}
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
