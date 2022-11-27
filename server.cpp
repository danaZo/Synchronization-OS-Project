#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unordered_map>
#include "Queue.hpp"
#include "active_object.hpp"

// Linux and other UNIXes
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


#define PORT "3334"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold

//Declaring the server's queue

pqueue server_queue, que2, que3;
pAO first_ao, second_ao, third_ao;

std::unordered_map<std::string,int> map;

void sendToClient(int sockid, char * message){
    int len = strlen(message);
    char str[1025] = "OUTPUT:";
    strcpy((str + 7),message);
    strcpy((str + 7 + len),"\n\0");
    if (send(sockid, str, strlen(str), 0) == -1){
        printf("send() failed with error code : %d" ,errno);
        close(sockid); 
    }
    close(sockid); 
}

//pass the item from the first active object to the queue of the second ao
void * after1(void * arg){
    size_t size = strlen((char*)arg);
    enQ(second_ao->que, arg, size);
    return nullptr;
}
//pass the item from the second active object to the queue of the third ao
void * after2(void * arg){
    size_t size = strlen((char*)arg);
    enQ(third_ao->que, arg, size);
    return nullptr;
}

void* sendItem(void * arg){

    char * str = (char*) arg;
    int soc = map.at(std::string(str));
    
    sendToClient(soc, str);

    return nullptr;
}




void* print(void * arg){

    char * str = (char*) arg;
    printf("%s\n", str);
    fflush(stdout);
    return NULL;
}

void * none(){
    return NULL;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *handle_cilent(void * arg);


int main(void)
{
    
    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo *servinfo, *p, hints;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
   
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1)
        {
            printf("Could not create listening socket : %d" ,errno);
            continue;
        }
        int enableReuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&enableReuse, sizeof(int)) < 0)
        {
        printf("setsockopt() failed with error code : %d" , errno);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            printf("Bind failed with error code : %d" ,errno);
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        printf("listen() failed with error code : %d",errno);
        close(sockfd);
        exit(1);
    }

    printf("server: waiting for connections...\n");
    pthread_t tid;

    server_queue = createQ(1000); //initiate the queue.
    que2 = createQ(1000);
    que3 = createQ(1000);
    first_ao = newAO(server_queue, caesar, after1);
    second_ao = newAO(que2, switchCase, after2);
    third_ao = newAO(que3, sendItem, print);
    while(1) { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            printf("listen failed with error code : %d",errno);
            
            close(sockfd);
            
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if( pthread_create(&tid, NULL, handle_cilent, &new_fd) != 0){
            perror("Thread creating failed");
        }
    }

    return 0;
}


void *handle_cilent(void * arg){
        
        int client_soc = *(int*) arg; //get sock id
        

        

        char message[1024];
        void* answer;
        memset(message, 0, 1024);
        char * CR_index;

        
        if(recv(client_soc, message, 1024, 0) < 0){
            printf("recv() failed with error code : %d" ,errno);
            
            close(client_soc);    
            
        }
        

        //replace CRLF with LF in case the os use it
        CR_index = strchr(message, '\r');
        if(CR_index){
            *CR_index = '\n';
            *(CR_index +1) = 0; //replace the old LF with null
        }


        // execute the  command
        if(strncmp("deQ", message, 3) == 0){

            answer = deQ(server_queue);
            if(!answer){ // queue is empty
                
                sendToClient(client_soc,"Queue is empty.\n");
                return arg;
            }
            
            sendToClient(client_soc,(char*)answer);
            
        }
        
        else if (strncmp("enQ ", message, 4) == 0){
            char * tok;
            tok = strtok(message, "\n");
            tok += 4;
            
            map[getKey(tok)] = client_soc;
            enQ(server_queue, tok, strlen(message));
        }
        
        else if (strcmp("destroyQ\n", message) == 0){
            destroyQ(server_queue);
        }
        
        return NULL;

}