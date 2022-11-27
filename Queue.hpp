#pragma once
#include <pthread.h>

typedef struct queue{

    int first, last, size;
    unsigned capacity;
    void ** arr;
    pthread_mutex_t slock;
    pthread_cond_t notempty;
    
}queue,*pqueue;

#define FALSE 0
#define TRUE 1

pqueue createQ(unsigned capacity);
void destroyQ(pqueue q);
int isFull(pqueue q);
int isEmpty(pqueue q);
void* deQ(pqueue q);
void enQ(pqueue q, void* toAdd, size_t size);
