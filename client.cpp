/* client.cpp */

#include "pq.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <cstdlib>

int client(string client_request, string IPC_path) {
    
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

    
    /* Send request to server */
    if (send(s, client_request.c_str(), client_request.size(), 0) == -1) {
        perror("Error in client sending request...");
        exit(EXIT_FAILURE);
    }

    /* Print out response from server */
    bool done = false;
    while (!done) {
        if ((t = recv(s, str, BUFSIZ, 0)) > 0) {
            str[t] = '\0';
            cout << str << endl;
            done = true;
        } else {
            if (t < 0) {
                perror ("Error in receiving server response...");
                exit(EXIT_FAILURE);
            } else {
                client_log("Request sent and handled");
                done = true;
            }
        }
    }


    return 0;
}
