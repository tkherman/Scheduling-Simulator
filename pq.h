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


/* Custom struct and enum declaration */
enum Request {
    ADD,
    STATUS,
    RUNNING,
    WAITING,
    FLUSH
};

struct Process {
    pid_t   pid;
    string  command;
    string  state;
    int     user_time;
    int     threshold;
    float   cpu_usage;
    time_t  arrival;
    time_t  start;
    int     priority;
};

enum Policy {
    FIFO,
    RDRB,
    MLFQ
};

struct Scheduler {
    queue<Process>          waiting_jobs;
    queue<Process>          running_jobs;
    vector<list<Process>>   levels;
    Policy                  policy;
    int                     ncpu;
    time_t                  last_called;
};



int client(string client_request, string IPC_path);

int server(int ncpu, Policy p, int time_slice, string IPC_path);
string handle_request(Scheduler &s, string request);
void schedule(Scheduler &s);
void sigchld_handler(int sig);


#endif
