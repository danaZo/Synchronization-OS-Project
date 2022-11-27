#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include "guard.hpp"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char * pointer; 

void * updateptr(void * ptr){
    
    guard g(&lock);
    pointer = (char *) ptr;
    sleep(2.5);
    return nullptr;
}

int main(){

    pthread_t t1, t2;
    char * p = "hi";
    pthread_create(&t1,NULL, updateptr, nullptr);
    pthread_create(&t2,NULL, updateptr, p);
    sleep(1); // we let the main thread sleep 1 second, then check that the value of pointer is still nullptr
    
    assert(pointer == nullptr); //this will throw if pointer is not null ptr

    // printf("%d\n" , pointer == nullptr );
    pthread_cancel(t2); // terminate the second thread

    return 0;
}