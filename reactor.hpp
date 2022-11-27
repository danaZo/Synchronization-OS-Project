#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <poll.h>
#include <vector>
#include <unordered_map>
using namespace std;


typedef struct reactor
{
    // vector<pollfd> pfds;
    unordered_map<int,void* (*)(void*)> funmap;
    struct pollfd *pfds;
    // void* (* fun[20])(void*);
    pthread_t loopThread;
    size_t size;
    size_t fd_count;
}reactor, *prec;

void * createReactor();

void * print(void*);

void installHandler(prec r, void *(*fun)(void*), int fd);
void RemoveHandler(prec r, int fd);

void * mainLoop(void * arg);

typedef struct node{

    prec r;
    int fd;

}node, *pnode;