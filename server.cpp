/* server.cpp */

/* This code borrows from the Beej Guide to Network Programming and
 * and IBM Knowledge center */

#include "pq.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>


/* This function sets up a listen socket and returns to main server function */
int set_up_socket(sockaddr_un &remote, string IPC_path) {
    int s, len;
    struct sockaddr_un local;
	
    /* Create socket */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, IPC_path.c_str());
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	/* Bind socket */
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("Failed to Bind");
		exit(EXIT_FAILURE);
	}

	/* Listen */
	if (listen(s, SOMAXCONN) == -1) {
		perror("Listen failure");
		exit(EXIT_FAILURE);
	}

    /* Return socket descriptor */
    return s;
}


/* This is the main event loop for the server. It sets up a IPC using unix
 * domain socket and continuous listen to potential request when calling
 * scheduler to handle the scheduling of the jobs */
int server(int ncpu, Policy p, uint64_t time_slice, string IPC_path) {
    
    /* Signal handling */
    struct sigaction int_action;
    int_action.sa_handler = &sigint_handler;
    sigemptyset(&int_action.sa_mask);
    if (sigaction(SIGINT, &int_action, 0) == -1) {
        perror(0);
        exit(EXIT_FAILURE);
    }
    
    struct sigaction sa;
    sa.sa_handler = &sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror(0);
        exit(EXIT_FAILURE);
    }

    int s, s2;
    socklen_t t;
    struct sockaddr_un remote;
    char buff[BUFSIZ];

    /* Initialize schduler struct */
    s_struct->ncpu = ncpu;
    s_struct->policy = p;
    s_struct->time_slice = time_slice;
   
    /* Create unix domain socket for listening */
    s = set_up_socket(remote, IPC_path);

	server_log("Waiting for connections from client...");

    /* Set up pollfd for poll() call */
    struct pollfd pfds = {s, POLLIN, 0};

	/* Loop to wait for connections */
	while(1) {
		
        int poll_result = poll(&pfds, 1, time_slice/1000);
        
        /* Check if poll call failed */
        if (poll_result < 0) {
            if (errno != EINTR) {
                perror("poll() failed");
            }
            continue;
        
        /* Check if there's a request from client */
        } else if (poll_result > 0) {
            t = sizeof(remote);
            if((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
                perror("Failed to accept");
                exit(EXIT_FAILURE);
            }
            server_log("Taking request from a client...");
            
			memset(&buff, 0, sizeof(buff));
            int bytes_recvd = recv(s2, buff, BUFSIZ, 0);
            if (bytes_recvd < 0) {
                perror("Server error receiving request");
            }
            
            string response = handle_request(string(buff));

            if (send(s2, response.c_str(), response.size(), 0) < 0) {
                perror("Server error sending response back to client");
            }
        }

        /* Call scheduler after polling, block SIGCHLD when calling schedule */
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        
        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
            perror("Error in sigprocmask");
        }
        schedule(time_slice);
        if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
            perror("Error in sigprocmask");
        }
	}

    return 0;
}
