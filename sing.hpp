#include<iostream>
#include<mutex>
#include<stdlib.h>
using namespace std;

template<typename T>
class Singleton {
public:

private:
    Singleton() {}
    Singleton(const Singleton&) {}
    Singleton& operator=(const Singleton&) {}
    static T* instance_;
    static mutex mutex_;
};
template<typename T>
T* Singleton<T>::instance_ = nullptr;
template<typename T>
mutex Singleton<T>::mutex_;