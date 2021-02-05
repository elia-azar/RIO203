#include <stdio.h>
#include "res-DryerMachine.h"

char* DryerMachine_state = "OFF";
float DryerMachine_consumption = 0;
float DryerMachine_on_Consumption = 3000;
float DryerMachine_off_Consumption = 0;

char * get_DryerMachine_state(){
    return DryerMachine_state;
}

float get_DryerMachine_consumption(){
    if (strncmp(DryerMachine_state, "ON", 2)){
        return DryerMachine_off_Consumption;
    }
    return DryerMachine_on_Consumption;
}

void change_DryerMachine_state(int new_state){
    if (new_state){
        DryerMachine_state = "ON";
    }else{
        DryerMachine_state = "OFF";
    }
    return;
}

float get_DryerMachine(){
    return 0;
}