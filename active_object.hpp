#pragma once
#include <string>
#include "Queue.hpp"

typedef struct AO{

    pqueue que;
    pthread_t thread;
    void* (* func)(void *);
    void* (* afterFunc)(void *);
}AO, *pAO;


void* handlePending(void * arg);

pAO newAO(pqueue que, void * (*func)(void *), void * (*afterFunc)(void *));

void destroyAO(pAO ao);


void * caesar(void * s);

void * switchCase(void *);

std::string getKey(char * str);