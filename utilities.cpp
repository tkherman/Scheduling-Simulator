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

//THINGS WE NEED TO GET FROM HERE
// - CPU USAGE
// - USER TIME
// - STATE (RUNNING or NOT)
