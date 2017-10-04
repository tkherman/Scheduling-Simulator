/* signal.cpp */

#include "pq.h"

#include <cstdlib>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>


/* This signal handler detects if a running child process has died.
 * If true, it updates the s_struct and call schedule() */
void sigchld_handler(int sig) {
    /* Declare variables */
    pid_t p;
    int status;

    /* Detects all dead child process and clean up */
    while ((p = waitpid(-1, &status, WNOHANG)) != -1) {
        
        /* Waitpid returns 0 if parent process has children that haven't changed state */ 
        if (p == 0) {
            break;
        }

        /* Access the Process struct of the process */
        Process *current_p = NULL;

        for (auto &pt : s_struct->running_jobs) {
            if (pt->pid == p) {
                current_p = pt;
                break;
            }
        }
        
        if (current_p == NULL) {
            for (auto &pt : s_struct->waiting_jobs) {
                if (pt->pid == p) {
                    current_p = pt;
                    break;
                }
            }
        }

        
        if (current_p == NULL) {
            server_log("Death of child process became a mystery");
            return;
        }
        
        /* Calculate and update the new average turnaround and response time */
        
        uint64_t turnaround = getCurrentTime() - current_p->arrival;
        uint64_t response = current_p->start - current_p->arrival;
        uint64_t total_t = s_struct->average_turnaround * s_struct->process_finished + turnaround;
        uint64_t total_r = s_struct->average_response * s_struct->process_finished + response;

        s_struct->process_finished++;
        
        s_struct->average_turnaround = total_t / s_struct->process_finished;
        s_struct->average_response = total_r / s_struct->process_finished;
        
        /* Log death of child process */
        server_log("Reaped process " << p << ": " << current_p->command << " Turnaround = "
                    << turnaround/1000 << "ms, Response = " << response/1000 << "ms");
        
        /* Free the Process struct */
        for (auto it = s_struct->running_jobs.begin(); it != s_struct->running_jobs.end(); it++) {
            if ((*it)->pid == p) {
                s_struct->running_jobs.erase(it);
                break;
            }
            
        }
        
        for (auto it = s_struct->waiting_jobs.begin(); it != s_struct->waiting_jobs.end(); it++) {
            if ((*it)->pid == p) {
                s_struct->waiting_jobs.erase(it);
                break;
            }
            
        }

        delete current_p;
    }
    
    /* Call schedule function */
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror("Error in sigprocmask");
    }
    schedule(0);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
        perror("Error in sigprocmask");
    }
}

/* This signal handler detects SIGINT. It frees all allocated memory,
 * kill all running processes and exit the program */
void sigint_handler(int sig) {

    empty_scheduler();

    delete s_struct;

    server_log("SIGINT caught. Now exiting program! Bye!");
    exit(EXIT_SUCCESS);
}
