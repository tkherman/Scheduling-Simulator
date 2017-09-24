/* pq.h */

#ifndef PQ_H
#define PQ_H

#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <list>
#include <ctime>

using namespace std;


/* Macros */
#define debug(M) \
    cerr << "[DEBUG]" << __FILE__ << ":" << __LINE__ << ":" << __func__ << ":" << M << endl;

#define client_log(M) \
    cout << M << endl;

#define server_log(M) \
    cout << "[" << time(NULL) << "] " << M << endl;

#define log(M) \
    cout << M << endl;

/* function to get current time in microseconds */
uint64_t getCurrentTime();

/* Custom struct and enum declaration */
struct Process {
    pid_t   pid;
    string  command;
    string  state;
    int     user_time;
    int     threshold;
    float   cpu_usage;
    time_t  arrival;
    time_t  start;

    Process(string cmd) {
        this->pid = 0;
        this->command = cmd;
        this->state = "sleeping";
        this->user_time = 0;
        this->threshold = 0;
        this->cpu_usage = 0.0;
        this->arrival = getCurrentTime();
        this->start = 0;
    };
};

enum Policy {
    FIFO,
    RDRB,
    MLFQ
};

struct Scheduler {
    // use deque as queue, push_front() for push, pop_back() for pop
    deque<Process*>         waiting_jobs; 
    deque<Process*>         running_jobs;
    vector<list<Process*>>  levels;
    Policy                  policy;
    int                     ncpu;
    uint64_t                last_called;
    float                   average_turnaround;
    float                   average_response;

    Scheduler() {
        this->policy = FIFO;
        this->ncpu = 1;
        this->last_called = getCurrentTime();
        this->average_turnaround = 0.0;
        this->average_response = 0.0;
    }

};

extern struct Scheduler *s_struct;


int client(string client_request, string IPC_path);
int server(int ncpu, Policy p, uint64_t time_slice, string IPC_path);
string handle_request(string request);
void schedule(uint64_t time_slice);
void sigchld_handler(int sig);
void free_scheduler();


#endif
