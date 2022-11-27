
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <errno.h>
#include <unistd.h>


#define SERVER_PORT 1234
#define LOCAL_HOST "127.0.0.1"



int main(){
    

    struct sockaddr_in serv;

    printf("Hi, wellcome. this is a basic chat app\n"
    "Any message you write will be sent to any online member\n");    
    
    char message[1024];
    memset(message, 0, 1024);

    scanf("%[^\n]", message);

    message[strlen(message)] = '\n';

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

    int bytesSent = send(soc,message, strlen(message), 0);
    if (-1 == bytesSent)
    {
        printf("send() failed with error code : %d" 
        
            ,errno
        
            );
    }
    else if (0 == bytesSent)
    {
	printf("peer has closed the TCP connection prior to send().\n");
    }

    memset(message, 0, 1024);
    recv(soc,message,1024,0);

    printf("%s",message);
    return 0;
}