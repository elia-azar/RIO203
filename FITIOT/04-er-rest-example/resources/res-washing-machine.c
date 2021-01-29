#include <stdio.h>
#include "res-washing-machine.h"

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