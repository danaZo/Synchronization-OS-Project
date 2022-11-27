#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Queue.hpp"
#include <unistd.h>



pqueue createQ(unsigned capacity)
{
    pqueue temp = (pqueue)malloc(sizeof(queue));
    if(!temp)
    {
        perror("queue allocation failed\n");
        exit(1);
    }

    temp->capacity = capacity;
    temp->first = temp->size = 0;
    temp->last = capacity -1;

    temp->arr = (void**)malloc(sizeof(void*) * temp->capacity);
    if(!temp->arr)
    {
        perror("allocation failed\n");
        exit(1);
    }

    // create a unique lock for the stack
    if(pthread_mutex_init(&temp->slock,NULL)){
        perror("Lock creation failed\n");
        exit(1);
    }

    if(pthread_cond_init(&temp->notempty, NULL)){
        perror("Condition creation faild\n");
        exit(1);
    }

    return temp;
}

int isFull(pqueue q)
{
    return(q->size == q->capacity);
}

int isEmpty(pqueue q)
{
    return(q->size == 0);
}



void* deQ(pqueue q)
{
    pthread_mutex_lock(&q->slock);

    while (isEmpty(q))
    {
        pthread_cond_wait(&q->notempty,&q->slock);
    }
    
    void* toDel = q->arr[q->first];
    q->first = (q->first+1) % q->capacity;
    q->size = q->size -1;

    pthread_mutex_unlock(&q->slock);
    
    return toDel;
}



void enQ(pqueue q, void* toAdd, size_t size)
{
    pthread_mutex_lock(&q->slock);

    if (isFull(q))
    {
        printf("queue is full, can't enqueue\n");
        pthread_mutex_unlock(&q->slock);
        exit(1);
    }
    void * entry = malloc(size);
    memcpy(entry, toAdd, size);
    q->last = (q->last+1) % q->capacity;
    q->arr[q->last] = entry;
    q->size = q->size + 1;

    pthread_mutex_unlock(&q->slock);
    pthread_cond_signal(&q->notempty);
}

/* free all the allocated memory we used to create the queue */
void destroyQ(pqueue q)
{
   free(q->arr);
   pthread_mutex_destroy(&q->slock);
   pthread_cond_destroy(&q->notempty);
   free(q);
    
}


