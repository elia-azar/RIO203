#include <stdio.h>
#include "res-Refrigerator.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


char* Refrigerator_state = "RESTING";
float Refrigerator_consumption = 0;
float Refrigerator_on_Consumption = 250;
float Refrigerator_off_Consumption = 0;

srand(time(0));

int random_number(){
    return rand() % 101;
} 

void initialize_refrigerator(){
    if(random_number() > 55){
        change_Refrigerator_state(1);
    }else{
        change_Refrigerator_state(0);
    }
    return;
}

char * get_Refrigerator_state(){
    return Refrigerator_state;
}

float get_Refrigerator_consumption(){
    if (strncmp(Refrigerator_state, "COOLING", 2)){
        return Refrigerator_off_Consumption;
    }
    return Refrigerator_on_Consumption;
}

void change_Refrigerator_state(int new_state){
    if (new_state){
        Refrigerator_state = "COOLING";
    }else{
        Refrigerator_state = "RESTING";
    }
    return;
}

float get_Refrigerator(){
    return 0;
}

void *refrigeratorThread(void *vargp){
    while(1){
        if(random_number() > 55){
            change_Refrigerator_state(1);
        }else{
            change_Refrigerator_state(0);
        }
        sleep(10 * 60);
    }
    return 0; 
}