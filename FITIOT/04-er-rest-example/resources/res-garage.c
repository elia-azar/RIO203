#include <stdio.h>
#include "res-garage.h"

char* garage_state = "Close";
float garage_consumption = 0;

char * get_garage_state(){
    return garage_state;
}

float get_garage_consumption(){
    
    return 0;
}

void change_garage_state(int new_state){
    if (new_state){
        garage_state = "Open";
    }else{
        garage_state = "Close";
    }
    return;
}

float get_garage(){
    return 0;
}