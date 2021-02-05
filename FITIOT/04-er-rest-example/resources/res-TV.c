#include <stdio.h>
#include "res-TV.h"

char* TV_state = "OFF";
float TV_consumption = 0;
float TV_on_Consumption = 100;
float TV_off_Consumption = 0;

char * get_TV_state(){
    return TV_state;
}

float get_TV_consumption(){
    if (strncmp(TV_state, "ON", 2)){
        return TV_off_Consumption;
    }
    return TV_on_Consumption;
}

void change_TV_state(int new_state){
    if (new_state){
        TV_state = "ON";
    }else{
        TV_state = "OFF";
    }
    return;
}

float get_TV(){
    return 0;
}