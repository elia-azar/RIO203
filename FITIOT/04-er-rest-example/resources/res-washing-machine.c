#include <stdio.h>
#include "res-washing-machine.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int random_number_washing_machine(){
    return rand() % 337;
} 

void initialize_washing_machine(){
    if(random_number_washing_machine() > 330){
        change_washing_machine_state(1);
    }else{
        change_washing_machine_state(0);
    }
    return;
}

char* washing_machine_state = "OFF";
float washing_consumption = 0;
float washing_on_Consumption = 500;
float washing_off_Consumption = 0;

char * get_washing_machine_state(){
    return washing_machine_state;
}

float get_washing_machine_consumption(){
    if (strncmp(washing_machine_state, "ON", 2)){
        return washing_off_Consumption;
    }
    return washing_on_Consumption;
}

void change_washing_machine_state(int new_state){
    if (new_state){
        washing_machine_state = "ON";
    }else{
        washing_machine_state = "OFF";
    }
    return;
}

float get_washing_machine(){
    return 0;
}

void *washing_machineThread(void *vargp){
    srand(time(0));
    while(1){
        if(random_number_washing_machine() > 330){
            change_washing_machine_state(1);
        }else{
            change_washing_machine_state(0);
        }
        sleep(30 * 60);
    }
    return 0; 
}