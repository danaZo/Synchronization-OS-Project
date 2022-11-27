/*
To run the tests: first run the server , then run test_server
each client can send only one command, after one command it closes.
*/


#include <assert.h> // for the tests

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h> // threads functions


#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <errno.h>
#include <unistd.h>


#define PORT 3334  // the port users will be connecting to
#define LOCAL_HOST "127.0.0.1"

#define BACKLOG 10   // how many pending connections queue will hold

 
int deq_soc, enq_soc;
char message[1024];

// also for dequeue and enqueue
void Enq_Soc(int client_num, char* command){

    struct sockaddr_in serv;

    enq_soc = socket(AF_INET, SOCK_STREAM, 0);
    if(enq_soc == -1)
    {
        printf("Could not create socket : %d" ,errno);
    }

    memset(&serv, 0, sizeof(serv));

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    serv.sin_port = htons(PORT);


    if (connect(enq_soc, (struct sockaddr *) &serv, sizeof(serv)) == -1)
    {
	    printf("connect() failed with error code : %d" ,errno);
    }

    printf("client %d connected to server\n", client_num);

    int bytesSent = send(enq_soc , command, 1024 , 0);
    if (-1 == bytesSent)
    {
        printf("send() failed with error code : %d"  ,errno );
    }
    else if (0 == bytesSent)
    {
	printf("peer has closed the TCP connection prior to send().\n");
    }

    if (recv(enq_soc, message, 1024, 0) == -1) {
        printf("recv() failed with error code : %d"  ,errno);
           
        close(deq_soc);
                
    }

    printf("%s\n", (char*)command);

    sleep(2);

}


int main()
{

    Enq_Soc(1, "enQ a"); // after the ao server print B
    assert(&message[0]);
    printf("%s\n", &message[0]);    
    assert(strncmp(&message[0], "OUTPUT:B", 8)==0);

    Enq_Soc(2, "enQ b"); //after the server print C
    assert(&message[0]);
    printf("%s\n", &message[0]);    
    assert(strncmp(&message[0], "OUTPUT:C", 8)==0);

    Enq_Soc(3, "enQ c"); //after the ao server print D
    assert(&message[0]);
    printf("%s\n", &message[0]);    
    assert(strncmp(&message[0], "OUTPUT:D", 8)==0);


    close(deq_soc); 

    
    close(enq_soc);
     
    return(0);

}
