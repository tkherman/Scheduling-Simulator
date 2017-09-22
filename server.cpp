/* server.cpp */

//This code borrows from the Beej Guide to Network Programming

#include "pq.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <cstdlib>

int server(int ncpu, Policy p, int time_slice, string IPC_path) {
    
    int s, s2, t, len;
	struct sockaddr_un local, remote;
	int buff_len = 256;
	char buff[buff_len];

    /* Create socket */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, IPC_path.c_str());
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	/* Bind */
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("Failed to Bind")
		exit(EXIT_FAILURE);
	}

	/* Listen */
	if (listen(s, SOMAXCONN) == -1) {
		perror("Listen failure");
		exit(EXIT_FAILURE);
	}

	/* Loop to wait for connections */
	while(1) {
		server_log("Waiting for connections");

		t = sizeof(remote);
		if((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
			perror("Failed to accept");
			exit(EXIT_FAILURE);
		}
		server_log("connected");
		
		done = false;
		while(!done) {
			int bytes_recvd = recv(s2, buff, buff_len, 0);
			if(bytes_recvd < 0) {
				done = true;
				perror("Error receiving");
			} else if {
				done = true;
			}

			if(!done) {
				//Process here
				string request = buff;

				//Initialize Scheduler
				//TODO
				handle_request(sched, request);
			}
		}

	}

    return 0;
}
