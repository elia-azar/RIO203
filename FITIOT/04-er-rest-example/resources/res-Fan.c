#include <stdio.h>
#include "res-Fan.h"
#include <time.h>
#include <unistd.h>

float t_outside = 0;
char* Fan_state = "OFF";
float Fan_consumption = 0;
float Fan_1_on_Consumption = 75;
float Fan_2_on_Consumption = 125;
float Fan_off_Consumption = 0;
const float START_FAN_1_AT = 15;
const float START_FAN_2_AT = 25;

float daily_temperature[] = {5, 4.8, 4, 3.1, 2.4, 4, 5.2, 6, 7.5, 9, 12, 16,
21, 26 , 30, 29, 26, 21, 17, 14.5, 12, 10, 7.5, 6};

void update_temp()
{
    time_t timer;
    time(&timer);
    struct tm *tm_struct = localtime(&timer);
    int hour = tm_struct->tm_hour;
    int minute = tm_struct->tm_min;
    // Updating t_outside
    float percentage = 1.0 - (float)minute / 60.0;
    t_outside = percentage * daily_temperature[hour] + (1.0 - percentage) * daily_temperature[(hour + 1) % 24];
    return;
}

void initialize_fan()
{
    update_temp();
    if (t_outside > START_FAN_2_AT){
        change_Fan_state(2);
    }else if (t_outside > START_FAN_1_AT){
        change_Fan_state(1);
    }
    return;
}

char * get_Fan_state(){
    return Fan_state;
}

float get_Fan_consumption(){
    if (!strncmp(Fan_state, "LEVEL_1", 7)){
        return Fan_1_on_Consumption;
    }else if (!strncmp(Fan_state, "LEVEL_2", 7)){
        return Fan_2_on_Consumption;
    }
    return Fan_off_Consumption;
}

void change_Fan_state(int new_state){
    if (1){
        Fan_state = "LEVEL_1";
    }else if (2){
        Fan_state = "LEVEL_2";
    }else{
        Fan_state = "OFF";
    }
    return;
}

float get_Fan(){
    return t_outside;
}

void *fanThread(void *vargp) 
{
    update_temp();
    while(1){
        if (t_outside > START_FAN_2_AT){
            change_Fan_state(2);
        }else if (t_outside > START_FAN_1_AT){
            change_Fan_state(1);
        }else{
            change_Fan_state(0);
        }
        sleep(3 * 60);
    }
    return 0; 
} 