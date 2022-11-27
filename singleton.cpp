#include <sys/mman.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include<fcntl.h> 
#include <assert.h>
#include "singleton.hpp"

/*
READ BEFORE CHECKING

since singelton is a template class it should be written inside
hpp file and not cpp file as you requested, so we put the test here
and the implemation in singleton.hpp
*/
class mapper{
public:
    int handle, len;
    char * file;
    mapper(std::string fname){
        struct stat size;
        fstat(handle,&size);
        len = size.st_size;
        handle = open(fname.c_str(),O_RDONLY | O_CREAT);
        file = (char*)mmap(NULL,size.st_size,PROT_READ,MAP_PRIVATE,handle,0);
    }
    mapper() = default;
};


int main(){ // lock a file using singleton
    // FILE *file = fopen("test_lock.txt","w+a+");

    mapper f1 = singleton<mapper>::instance("file1.txt");
    mapper f2 = singleton<mapper>::instance("file2.txt");

    //make sure that we are reading the same file
    assert(f1.len == f2.len);

    for(int i = 0 ; i <f1.len; i++){
        //compare files char by char
        assert(f1.file[i] == f2.file[i]);
    }

    return 0;
}
