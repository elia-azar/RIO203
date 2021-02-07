#include <stdio.h>
#include "res-stereo.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

char* stereo_state = "OFF";
float stereo_consumption = 0;
float stereo_on_Consumption = 40;
float stereo_off_Consumption = 0;

srand(time(0));

int random_number(){
    return rand() % 101;
}

void initialize_stereo(){
    if(random_number() > 97){
        change_stereo_state(1);
    }else{
        change_stereo_state(0);
    }
    return;
}

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

void *stereoThread(void *vargp){
    while(1){
        if(random_number() > 97){
            change_stereo_state(1);
        }else{
            change_stereo_state(0);
        }
        sleep(10 * 60);
    }
    return 0; 
}