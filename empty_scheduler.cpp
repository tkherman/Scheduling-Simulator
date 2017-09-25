/* empty_scheduler.cpp */

#include "pq.h"

#include <csignal>

void empty_scheduler() {
    
    /* Kill all processes and delete all Process struct */
    for (auto &p : s_struct->waiting_jobs) {
        
        /* If a process has been spawned for the job */
        if (p->pid != 0) {
            if (kill(p->pid, SIGTERM) == -1)
                perror("Error terminating process");
        }

        /* Free the process struct */
        delete p;
    }

    for (auto &p : s_struct->running_jobs) {
        
        /* If a process has been spawned for the job */
        if (p->pid != 0) {
            if (kill(p->pid, SIGTERM) == -1)
                perror("Error terminating process");
        }

        /* Free the process struct */
        delete p;
    }
    
    /* Clear levels vector */
    for (auto &level : s_struct->levels) {
        level.clear();
    }

    return;

}
