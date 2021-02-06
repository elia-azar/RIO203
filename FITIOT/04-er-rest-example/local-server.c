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
#include "resources/res-washing-machine.h"
#include "resources/temperature.h"
#include "resources/lamp.h"

int REST_MAX_CHUNK_SIZE = 13;

void *lamp(void *vargp){
    initialize_lamp(1);
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, lampThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void *heater(void *vargp) {
    initialize_heater();
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, heaterThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void *washing_machine(void *vargp) {
    /*pthread_t thread_id; 
    pthread_create(&thread_id, NULL, washing_machineThread, NULL); 
    pthread_join(thread_id, NULL);*/
    return;
}

void parser(char* buffer, char* buffer_to_send, int read_size){
    if(!strncmp(buffer, "washing_machine", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_washing_machine());
    }else if (!strncmp(buffer, "washing_machine/consumption", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_washing_machine_consumption());
    }else if (!strncmp(buffer, "washing_machine/state", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_washing_machine_state());
    }else if(!strncmp(buffer, "heater", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_temperature());
    }else if (!strncmp(buffer, "heater/consumption", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_heater_consumption());
    }else if (!strncmp(buffer, "heater/state", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_heater_state());
    }else if(!strncmp(buffer, "lamp", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_lux(1));
    }else if (!strncmp(buffer, "lamp/consumption", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_lamp_consumption());
    }else if (!strncmp(buffer, "lamp/state", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_lamp_state());
    }else if (!strncmp(buffer, "power_meter", read_size)){
        float consumpt = get_heater_consumption() + get_lamp_consumption() + get_washing_machine_consumption();
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", consumpt);
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
    pthread_t thread_lamp; 
    pthread_create(&thread_lamp, NULL, lamp, NULL); 
    pthread_t thread_heater; 
    pthread_create(&thread_heater, NULL, heater, NULL); 
    pthread_t thread_washing_machine; 
    pthread_create(&thread_washing_machine, NULL, washing_machine, NULL);

    // Run the server
    int sockfd, c, read_size, client_sock; 
    char buffer[100];
    int port=8877;
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
