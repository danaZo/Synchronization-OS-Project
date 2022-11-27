#include "reactor.hpp"


void * createReactor(){

    prec r = new reactor();
    r->pfds = (pollfd*) malloc(sizeof(pollfd) * 10);
    r->fd_count = 0;
    r->size = 10;

    // start polling the fds indefintly
    pthread_create(&r->loopThread, NULL, mainLoop,r);
    

    return r;
}






void * mainLoop(void * reac){

    prec r = (prec) reac;
    while(true){


        int polled = poll(&r->pfds[0],(nfds_t) r->fd_count, -1);

        if(polled < 0){
            perror("polling failed");
            exit(1);
        }
        //if we got here, at least one fd has data pending
        //now we should find which one is it
        for(int i = 0 ; i < r->fd_count; i++){

            if(r->pfds[i].revents & POLLIN){ //this one has data waiting

                //send the fd to its handler function
                pnode n = (pnode) malloc(sizeof(node));
                n->fd = r->pfds[i].fd;
                n->r = r;
                r->funmap.at(r->pfds[i].fd)(n);
            }

        }

    }


    return nullptr;
}

void installHandler(prec r, void *(*fun)(void*), int fd){

    r->pfds[r->fd_count].events = POLLIN;
    r->pfds[r->fd_count++].fd = fd;
    r->funmap.insert({fd, fun});
}

void RemoveHandler(prec r, int fd){

    //find the index of fd in pfds
    int index = 0;
    for(int i = 0; i < r->fd_count; i++){
        if(r->pfds->fd == fd){
            index = i;
            break;
        }
    }
    //remove the fd
    if(index == 0 && r->fd_count == 1){
        r->pfds[0].fd = -1;
        r->fd_count = 0;
    }
    else{
        r->pfds[index] = r->pfds[r->fd_count -1];
        r->fd_count--;
    }
    r->funmap.erase(fd);
}

void * print(void* arg){

    pnode n = (pnode) arg;
    char buff[256];
    bzero(buff,256);
    recv(n->fd,buff, 256, 0);
    close(n->fd);
    printf("message receieved: %s", buff);
    fflush(stdout);
    free(arg);

    return nullptr;
    
}