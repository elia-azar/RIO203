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
#include "resources/res-stereo.h"
#include "resources/res-TV.h"
#include "resources/res-Fan.h"

int REST_MAX_CHUNK_SIZE = 100;

void *fan(void *vargp){
    initialize_fan(1);
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, fanThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void *tv(void *vargp) {
    initialize_tv();
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, tvThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void *stereo(void *vargp) {
    initialize_stereo();
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, stereoThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

void parser(char* buffer, char* buffer_to_send, int read_size){
    if(!strncmp(buffer, "get/stereo", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_stereo());
    }else if (!strncmp(buffer, "consumption/stereo", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_stereo_consumption());
    }else if (!strncmp(buffer, "state/stereo", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_stereo_state());
    }else if(!strncmp(buffer, "get/tv", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_TV());
    }else if (!strncmp(buffer, "consumption/tv", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_TV_consumption());
    }else if (!strncmp(buffer, "state/tv", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_TV_state());
    }else if(!strncmp(buffer, "get/fan", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_Fan());
    }else if (!strncmp(buffer, "consumption/fan", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", get_Fan_consumption());
    }else if (!strncmp(buffer, "state/fan", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%s", get_Fan_state());
    }else if (!strncmp(buffer, "get/power_meter", read_size)){
        float consumpt = get_TV_consumption() + get_Fan_consumption() + get_stereo_consumption();
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", consumpt);
    }else if (!strncmp(buffer, "consumption/power_meter", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "%f", 0);
    }else if (!strncmp(buffer, "state/power_meter", read_size)){
        snprintf((char*)buffer_to_send, REST_MAX_CHUNK_SIZE, "ON");
    }else if(!strncmp(buffer, "everything", read_size)){
        /*char *answer = ;
        sprintf("washing-machine_value:%f_consumption:%f_state:%s||tv\
        _value:%f_consumption:%f_state:%s||fan_value:%f_consumption:%f_state:%s\
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
    pthread_t thread_fan; 
    pthread_create(&thread_fan, NULL, fan, NULL); 
    pthread_t thread_tv; 
    pthread_create(&thread_tv, NULL, tv, NULL); 
    pthread_t thread_stereo; 
    pthread_create(&thread_stereo, NULL, stereo, NULL);

    // Run the server
    int sockfd, c, read_size, client_sock; 
    char buffer[100];
    int port=8888;
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
