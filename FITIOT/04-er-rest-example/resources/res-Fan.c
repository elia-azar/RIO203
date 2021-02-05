#include <stdio.h>
#include "res-Fan.h"

char* Fan_state = "OFF";
float Fan_consumption = 0;
float Fan_on_Consumption = 75;
float Fan_off_Consumption = 0;

char * get_Fan_state(){
    return Fan_state;
}

float get_Fan_consumption(){
    if (strncmp(Fan_state, "ON", 2)){
        return Fan_off_Consumption;
    }
    return Fan_on_Consumption;
}

void change_Fan_state(int new_state){
    if (new_state){
        Fan_state = "ON";
    }else{
        Fan_state = "OFF";
    }
    return;
}

float get_Fan(){
    return 0;
}