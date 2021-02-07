#include <stdio.h>
#include "res-garage.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

char* garage_state = "CLOSED";
float garage_consumption = 0;

int random_number_garage(){
    return rand() % 101;
} 

void initialize_garage(){
    if(random_number_garage() > 90){
        change_garage_state(1);
    }else{
        change_garage_state(0);
    }
    return;
}


char * get_garage_state(){
    return garage_state;
}

float get_garage_consumption(){
    
    return 0;
}

void change_garage_state(int new_state){
    if (new_state){
        garage_state = "OPENED";
    }else{
        garage_state = "CLOSED";
    }
    return;
}

float get_garage(){
    return 0;
}

void *garageThread(void *vargp){
    srand(time(0));
    while(1){
        if(random_number_garage() > 90){
            change_garage_state(1);
        }else{
            change_garage_state(0);
        }
        sleep(10 * 60);
    }
    return 0; 
}