#pragma once
#include <pthread.h>


template <typename T> class singleton
{
private:
    static  pthread_mutex_t lock;
    static T* sing;
    //Make the 3 compiler provided constructors private so that no one can create an instance of singelton
    singleton() = default;
    singleton(const singleton&) =default;
    singleton& operator=(const singleton&) = default;
public:

    static T& instance(std::string fname = ""){
        pthread_mutex_lock(&lock);

        if(sing == nullptr){
            if(fname == ""){
                sing = new T();
            }
            else{
                sing = new T(fname);
            }
        }
        pthread_mutex_unlock(&lock);
        return *sing;
        
    }

    static void destroy(){
        delete sing;
        sing = nullptr;
    }

};

//initialize the static variables
template <typename T>
T* singleton<T>::sing = nullptr;
template<typename T> 
pthread_mutex_t singleton<T>::lock = PTHREAD_MUTEX_INITIALIZER;


