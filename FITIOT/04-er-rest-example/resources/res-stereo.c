#include <stdio.h>
#include "res-stereo.h"

char* stereo_state = "OFF";
float stereo_consumption = 0;
float stereo_on_Consumption = 40;
float stereo_off_Consumption = 0;

char * get_stereo_state(){
    return stereo_state;
}

float get_stereo_consumption(){
    if (strncmp(stereo_state, "ON", 2)){
        return stereo_off_Consumption;
    }
    return stereo_on_Consumption;
}

void change_stereo_state(int new_state){
    if (new_state){
        stereo_state = "ON";
    }else{
        stereo_state = "OFF";
    }
    return;
}

float get_stereo(){
    return 0;
}