#include <stdio.h>
#include "res-Refrigerator.h"

char* Refrigerator_state = "OFF";
float Refrigerator_consumption = 0;
float Refrigerator_on_Consumption = 250;
float Refrigerator_off_Consumption = 0;

char * get_Refrigerator_state(){
    return Refrigerator_state;
}

float get_Refrigerator_consumption(){
    if (strncmp(Refrigerator_state, "ON", 2)){
        return Refrigerator_off_Consumption;
    }
    return Refrigerator_on_Consumption;
}

void change_Refrigerator_state(int new_state){
    if (new_state){
        Refrigerator_state = "ON";
    }else{
        Refrigerator_state = "OFF";
    }
    return;
}

float get_Refrigerator(){
    return 0;
}