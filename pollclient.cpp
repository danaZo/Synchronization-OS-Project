#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <errno.h>
#include <unistd.h>

#define SERVER_PORT 1234
#define LOCAL_HOST "127.0.0.1"

void* recvFromServer(void * arg){
    while(true){
        char message[1024];
        memset(message, 0, 1024);

        int e = recv(*((int*)arg),message, 1024, 0 );
        if(e <= 0){
            exit(1);
        }
        printf("message receieved: ");
        printf("%s",message);
    }
    return nullptr;
}


int main(){

 struct sockaddr_in serv;

    printf("Hi, wellcome. this is a basic chat app\n"
    "Any message you write will be sent to any online member\n");    
    

    int soc;
    soc = socket(AF_INET,SOCK_STREAM, 0);
    if (soc == -1) 
    {
        printf("Could not create socket : %d" 
        
            ,errno
        
            );
    }

    memset(&serv, 0, sizeof(serv));
    
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    serv.sin_port = htons(SERVER_PORT);

    if(connect(soc, (struct sockaddr*) &serv, sizeof(serv)) < 0)
    {
        printf("connect() failed with error code : %d"  
            ,errno
            );
    }

    //thread that handles the receieved messages
    pthread_t sending_thread;
    pthread_create(&sending_thread, NULL, recvFromServer, &soc);


    //handle the sending in the main thread
    char message[1024];
    while(true){
        
        memset(message, 0, 1024);

        scanf("%s", message);
        message[strlen(message)] = '\n';
        send(soc, message, strlen(message), 0);
        printf("message sent\n");
    }
    return 0;
}
