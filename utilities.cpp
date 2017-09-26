/* utilities.cpp */
/* This file holds miscellaneous functions useful throughout the project */

#include "pq.h"
#include <sys/time.h>
#include <csignal>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include <fstream>

uint64_t getCurrentTime() {
	/* get current time in microseconds */
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t curr_time = 1000000 * tv.tv_sec + tv.tv_usec;
	return curr_time;
}

void empty_scheduler() {
    
    /* Kill all processes and delete all Process struct */
    for (auto &p : s_struct->waiting_jobs) {
        
        /* If a process has been spawned for the job */
        if (p->pid != 0) {
            if (kill(p->pid, SIGTERM) == -1)
                perror("Error terminating process");
        }
    }

    for (auto &p : s_struct->running_jobs) {
        
        /* If a process has been spawned for the job */
        if (p->pid != 0) {
            if (kill(p->pid, SIGTERM) == -1)
                perror("Error terminating process");
        }
    }
    
    /* Clear levels vector */
    for (auto &level : s_struct->levels) {
        level.clear();
    }

    return;

}

Process_Stat get_process_stat(pid_t pid) {
    
    Process_Stat stat;

    /* Get system uptime */
    float uptime;

    string system_file = "/proc/uptime";
    ifstream system_ifs(system_file, ifstream::in);
    
    system_ifs >> uptime;
    system_ifs.close();

    /* Open stat file */
    float utime, stime, starttime;
    string state;

    string filename = "/proc/" + to_string((int) pid) + "/stat";
    ifstream stat_ifs(filename, ifstream::in);

    string temp;
    for (int i = 1; i <= 22; i++) {
        stat_ifs >> temp;
        if (i == 3) state = temp;
        else if (i == 14) utime = stof(temp);
        else if (i == 15) stime = stof(temp);
        else if (i == 22) starttime = stof(temp);
    }
    
    stat_ifs.close();

    /* Calculate CPU usage */
    float hertz = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime;
    float seconds = uptime - (starttime / hertz);

    /* Fill struct */
    stat.cpu_usage = 100 * ((total_time/hertz) / seconds);
    stat.user_time = utime;
    if (!state.compare("R"))
        stat.state = "running";
    else if (!state.compare("S"))
        stat.state = "sleeping";
    else if (!state.compare("D"))
        stat.state = "sleeping";
    else if (!state.compare("Z"))
        stat.state = "zombie";

    return stat;
}

//THINGS WE NEED TO GET FROM HERE
// - CPU USAGE
// - USER TIME
// - STATE (RUNNING or NOT)
