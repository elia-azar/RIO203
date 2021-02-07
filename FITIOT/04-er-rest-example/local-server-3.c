#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "resources/res-Refrigerator.h"
#include "resources/res-DryerMachine.h"
#include "resources/res-garage.h"

int REST_MAX_CHUNK_SIZE = 100;

void *garage(void *vargp){
    initialize_garage(1);
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, garageThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void *dryer(void *vargp) {
    initialize_dryer();
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, dryerThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void *refrigerator(void *vargp) {
    initialize_refrigerator();
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, refrigeratorThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void parser(char* buffer, char* buffer_to_send, int read_size){
    if(!strncmp(buffer, "get/refrigerator", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_Refrigerator());
    }else if (!strncmp(buffer, "consumption/refrigerator", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_Refrigerator_consumption());
    }else if (!strncmp(buffer, "state/refrigerator", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_Refrigerator_state());
    }else if(!strncmp(buffer, "get/dryer", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_DryerMachine());
    }else if (!strncmp(buffer, "consumption/dryer", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_DryerMachine_consumption());
    }else if (!strncmp(buffer, "state/dryer", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_DryerMachine_state());
    }else if(!strncmp(buffer, "get/garage", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_garage(1));
    }else if (!strncmp(buffer, "consumption/garage", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_garage_consumption());
    }else if (!strncmp(buffer, "state/garage", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_garage_state());
    }else if (!strncmp(buffer, "get/power_meter", read_size)){
        float consumpt = get_DryerMachine_consumption() + get_garage_consumption() + get_Refrigerator_consumption();
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", consumpt);
    }else if (!strncmp(buffer, "consumption/power_meter", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", 0);
    }else if (!strncmp(buffer, "state/power_meter", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "ON");
    }else if(!strncmp(buffer, "everything", read_size)){
        /*char *answer = ;
        sprintf("washing-machine_value:%f_consumption:%f_state:%s||dryer\
        _value:%f_consumption:%f_state:%s||garage_value:%f_consumption:%f_state:%s\
        power_meter_value:%f\n",);*/
    }else{
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "Bad Request");
    }
    return;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[100];
     
    read_size = recv(sock , client_message , 100 , 0);

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    printf("%i, %s\n", read_size, client_message);
    //Send the message back to client
    char buffer_to_send[64];
    parser(client_message, buffer_to_send, read_size);
    printf("buffer to send: %s\n", buffer_to_send);
    write(sock , buffer_to_send , strlen(buffer_to_send));
         
    return 0;
} 

int main(){
    //Create Objects
    pthread_t thread_garage; 
    pthread_create(&thread_garage, NULL, garage, NULL); 
    pthread_t thread_dryer; 
    pthread_create(&thread_dryer, NULL, dryer, NULL); 
    pthread_t thread_refrigerator; 
    pthread_create(&thread_refrigerator, NULL, refrigerator, NULL);

    // Run the server
    int sockfd, c, read_size, client_sock; 
    char buffer[100];
    int port=8899;
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
      
    //Listen
	listen(sockfd , 5);
	
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id[5];
    int i = 0;
    
    //Receive a message from client
    while(client_sock = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t*)&c))
    {   
        printf("Connection accepted, served by thread: %i\n",i);

        if( pthread_create(&thread_id[i] , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        
        i = (i+1) % 5;
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
    
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

	
    return 0;
}
