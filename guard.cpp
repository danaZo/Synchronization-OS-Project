#include "guard.hpp"
guard::guard(pthread_mutex_t * l)
{
    lock = l;
    pthread_mutex_lock(l);
}

guard::~guard()
{
    pthread_mutex_unlock(lock);
}
