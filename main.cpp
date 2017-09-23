/* main.cpp */

#include "pq.h"

#include <cstdlib>


/* Usage function that prints out usage guide and exit according to status */
void usage(string program, int status) {
    cerr << "Usage: " << program << "[options]" << endl;
    cerr << endl;
    cerr << "General Options:" << endl;
    cerr << "   -h                  Print this helper message" << endl;
    cerr << "   -f                  Path to IPC channel" << endl;
    cerr << endl;
    cerr << "Client Options:" << endl;
    cerr << "   add COMMAND         Add COMMAND to queue" << endl;
    cerr << "   status              Query status of queue" << endl;
    cerr << "   running             Query running jobs" << endl;
    cerr << "   waiting             Query waiting jobs" << endl;
    cerr << "   flush               Remove all jobs from queue" << endl;
    cerr << endl;
    cerr << "Server Options:" << endl;
    cerr << "   -n NCPUS            Number of CPUs" << endl;
    cerr << "   -p POLICY           Scheduling policy (fifo, rdrn, mlfq)" << endl;
    cerr << "   -t MICROSECONDS     Time between scheduling" << endl;
    cerr << endl;

    exit(status);
}


/* Main execution */
int main(int argc, char *argv[]) {
    
    /* Initialize parameter to default */
    string IPC_path = "/tmp/communicatation_socket";

    /* Client side parameter */
    string client_request;

    /* Server side parameter */
    int ncpus = 1;
    int time_slice = 1000;
    Policy policy = FIFO;
    bool is_server = true;

    /* Parse command line arguments */ 
    string program = argv[0];
    
    int argind = 1;

    while (argind < argc) {
        string arg = argv[argind++];

        if (!arg.compare("-h")) {
            usage(program, 0);
        
        } else if (!arg.compare("-f")) {
            if (argind < argc) {
                IPC_path = argv[argind++];
            } else {
                log("Error: -f requires a path to IPC channel");
                usage(program, 1);
            }

        } else if (!arg.compare("add")) {
            if (argind < argc) {
                client_request = arg + " " + argv[argind++];
                is_server = false;
            } else {
                log("Error: add requires a COMMAND");
                usage(program, 1);
            }

        } else if (!arg.compare("status")) {
            client_request = "status";
            is_server = false;

        } else if (!arg.compare("running")) {
            client_request = "running";
            is_server = false;

        } else if (!arg.compare("waiting")) {
            client_request = "waiting";
            is_server = false;

        } else if (!arg.compare("flush")) {
            client_request = "flush";
            is_server = false;

        } else if (!arg.compare("-n")) {
            if (argind < argc) {
                ncpus = atoi(argv[argind++]);
            } else {
                log("Error: -n requires a number for cpus");
                usage(program, 1);
            }

        } else if (!arg.compare("-p")) {
            if (argind < argc) {
                arg = argv[argind++];
                if (!arg.compare("fifo"))
                    policy = FIFO;
                else if (!arg.compare("rdrn"))
                    policy = RDRB;
                else if (!arg.compare("mlfq"))
                    policy = MLFQ;
                else {
                    log("Error: -p please enter a valid policy");
                    usage(program, 1);
                }
            } else {
                log("Error: -p requires a policy");
                usage(program, 1);
            }

        } else if (!arg.compare("-t")) {
            if (argind < argc) {
                time_slice = atoi(argv[argind++]);
            } else {
                log("Error: -t requires a time in microseconds");
                usage(program, 1);
            }

        } else {
            log("Error: invalid argument " << arg);
            usage(program, 1);

        }
    }
    
    /* User calling program as client */
    if (!is_server) {
        client(client_request, IPC_path);
        exit(EXIT_SUCCESS); // If client returns, it means request was sent successfully
    /* User calling program as server */
    } else {
        server(ncpus, policy, time_slice, IPC_path);
    }
}
