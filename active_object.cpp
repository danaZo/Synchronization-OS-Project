#include <stdlib.h>
#include<string.h>
#include<string>
#include<cctype>
#include "active_object.hpp"



void* handlePending(void * arg){
    
    pAO ao = (pAO) arg; 
    while(true){

        void * item = deQ(ao->que);
        ao->func(item);
        ao->afterFunc(item);
    }
}

//create a new ao and thread to execute the given function on any pending item
pAO newAO(pqueue que, void * (*func)(void *), void * (*afterFunc)(void *)){

    pAO ao = (pAO) malloc(sizeof(AO));
    ao->que = que;
    ao->func = func;
    ao->afterFunc = afterFunc;
    pthread_create(&ao->thread, NULL, handlePending,ao );
    return ao;
}

//free the active objects and cancel the thread executing it
void destroyAO(pAO ao){

    pthread_cancel(ao->thread);
    free(ao);

} 


//functions that are used by active objects in Ex6

void * caesar(void * s){

    char * str = (char*)s;
    
    for(int i = 0; i < strlen(str); i++){

        if(isupper(str[i])){
            
            if(str[i] == 'Z'){
                str[i] = 'A';
            }
            else{
                str[i] += 1;
            }
        }

        else{
            if(str[i] == 'z'){
                str[i] = 'a';
            }
            else{
                str[i] += 1;
            }     
        }
    }
    return nullptr;
}

void * switchCase(void * arg){

    char * str = (char*) arg;

    for(size_t i = 0 ;i < strlen(str); i++ ){
        if(isupper(str[i])){
            str[i] = tolower(str[i]);
        }
        else{
            str[i] = toupper(str[i]);
        }
    }
    return nullptr;
}


std::string getKey(char * str){

    char temp[strlen(str) +1];
    temp[strlen(str)] = '\0';
    strcpy(temp,str);

    caesar(temp);
    switchCase(temp);
    return std::string(temp);
}
