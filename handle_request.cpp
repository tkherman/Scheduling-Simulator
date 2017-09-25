/* handle_request.cpp */

#include "pq.h"

#include <sstream>
#include <iomanip>
#include <csignal>

/* Allocate a new Process struct and fill in initialize info */
string add_job(string command){
     Process *new_p = new Process(command);
     s_struct->waiting_jobs.push_front(new_p);

     string response = "Added process: " + command;
     return response;
}

/* The next four functions access s_struct for process informations and return
 * a formatted string using stringstream */
string running_jobs() {
    stringstream rs;

    rs << setw(5) << "PID";
    rs << setw(14) << "COMMAND";
    rs << setw(8) << "STATE";
    rs << setw(8) << "USER";
    rs << setw(8) << "THRESHOLD";
    rs << setw(8) << "USAGE";
    rs << setw(12) << "ARRIVAL";
    rs << setw(12) << "START";
    rs << endl;
    for (auto &p : s_struct->running_jobs) {
        rs << setw(5) << p->pid;
        rs << setw(14) << p->command;
        rs << setw(8) << p->state;
        rs << setw(8) << p->user_time;
        rs << setw(8) << p->threshold;
        rs << setw(8) << setprecision(2) << p->cpu_usage;
        rs << setw(12) << p->arrival;
        rs << setw(12) << p->start;
        rs << endl;
    }

    return rs.str();
}

string waiting_jobs() {
    stringstream rs;

    rs << setw(5) << "PID";
    rs << setw(14) << "COMMAND";
    rs << setw(8) << "STATE";
    rs << setw(8) << "USER";
    rs << setw(8) << "THRESHOLD";
    rs << setw(8) << "USAGE";
    rs << setw(12) << "ARRIVAL";
    rs << setw(12) << "START";
    rs << endl;
    for (auto &p : s_struct->waiting_jobs) {
        rs << setw(5) << p->pid;
        rs << setw(14) << p->command;
        rs << setw(8) << p->state;
        rs << setw(8) << p->user_time;
        rs << setw(8) << p->threshold;
        rs << setw(8) << setprecision(2) << p->cpu_usage;
        rs << setw(12) << p->arrival;
        rs << setw(12) << p->start;
        rs << endl;
    }

    return rs.str();

}

string status_check() {
    stringstream rs;
    
    rs << "Running = " << s_struct->running_jobs.size() << ", Waiting = " << s_struct->waiting_jobs.size();
    rs << ", Turnaround = " << s_struct->average_turnaround << ", Response = " << s_struct->average_response << endl;
    rs << endl;
    rs << "Running Queue:" << endl;
    rs << running_jobs();
    rs << endl;
    rs << "Waiting Queue:" << endl;
    rs << waiting_jobs();

    return rs.str();
}

string flush_jobs() {
    stringstream rs;

    rs << "Flushed " << s_struct->running_jobs.size() << " running and ";
    rs << s_struct->waiting_jobs.size() << " waiting processes" << endl;
    
    empty_scheduler();

    return rs.str();
}



/* This function process the request and return a string that is sent back to
 * the client
 *  - it will parse the request then call appropriate helper function */
string handle_request(string request) {
    
    string response;
    
    /* Parse request string */
    string reader;
    stringstream ss(request);
    
    ss >> reader;
    
    if (!reader.compare("add")) {
        string command;
        ss >> command;
        response = add_job(command);
    }
    else if (!reader.compare("status"))
        response = status_check();

    else if (!reader.compare("running"))
        response = running_jobs();

    else if (!reader.compare("waiting"))
        response = waiting_jobs();
    
    else if (!reader.compare("flush"))
        response = flush_jobs();

    return response;
}
