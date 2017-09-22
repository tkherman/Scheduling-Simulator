/* client.cpp */

#include "pq.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <cstdlib>

int client(Request client_request, string command, string IPC_path) {
    
    int s, t, len;
    struct sockaddr_un remote;
    char str[BUFSIZ];
    
    /* Create socket */
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Fail to create a socket");
        exit(EXIT_FAILURE);
    }

    client_log("Trying to connect to server...");
    
    /* Attempt to connect to server */
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, IPC_path.c_str());
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr*) &remote, len) == -1) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    client_log("Successfully connected to server...");

    /* Determine request to send to server */
    string request_str;
    switch (client_request) {
        case ADD:
            request_str = "add " + command;
            break;
        case STATUS:
            request_str = "status";
            break;
        case RUNNING:
            request_str = "running";
            break;
        case WAITING:
            request_str = "waiting";
            break;
        case FLUSH:
            request_str = "flush";
            break;
    }




    
    return 0;
}
