#include <pthread.h>


// answer for the question:
// no - the scope guard cannot protect strtok when using multiple threads  

class guard
{
private:
    pthread_mutex_t * lock;
public:
    guard(pthread_mutex_t*);
    ~guard();
};