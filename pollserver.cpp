#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "reactor.hpp"


#define PORT "1234"   // Port we're listening on
int listener; // used to handle new connections
char remoteIP[INET6_ADDRSTRLEN]; // store client IP
// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

void * sendMsg(void * arg){
    //this function read message from one client and send it to all the other clients
    
    pnode n = (pnode) arg;
    prec r = n->r;
    char buff[256];
    bzero(buff,256);

    recv(n->fd, buff, 256, 0);
    //read the message
    for(int i = 0 ; i < r->fd_count; i++){

        if(r->pfds[i].fd != listener && r->pfds[i].fd != n->fd){//dont send to the listener and the original sender
            send(r->pfds[i].fd, buff, strlen(buff), 0);
        }
    }

    return nullptr;
}


void * newClient(void * arg){
    pnode n = (pnode) arg;
    prec r = n->r;
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen = sizeof remoteaddr;
    int newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);

    printf("%d\n", newfd);
    installHandler(r, sendMsg, newfd );
    printf("pollserver: new connection from %s on "
    "socket %d\n",
    inet_ntop(remoteaddr.ss_family,
        get_in_addr((struct sockaddr*)&remoteaddr),
        remoteIP, INET6_ADDRSTRLEN),
    newfd);

    return nullptr;
}



int main(){

    //create the listener socket
    
    listener = get_listener_socket();

    //create the reactor
    prec rec = (prec) createReactor();
    installHandler(rec, newClient, listener);
    pthread_join(rec->loopThread, NULL);



return 0;

}