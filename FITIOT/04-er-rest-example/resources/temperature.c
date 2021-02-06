#include <stdio.h>
#include "temperature.h"
#include "common.h"
#include <time.h>
#include <pthread.h> 
#include <unistd.h>

float t_outside = 0;
float t_inside = 0;
float t_heater = 0;
int heater_state = 0;
int daytime_minutes = 0;
const float COOLING = 0.6;
const float HEATING = 1;
const float MAX_HEATER_TEMP = 60.0;

const float START_HEATING_AT = 12.0;
const float STOP_HEATING_AT = 22.0;

const float HEATER_ON_CONSUMPTION = 7500;
const float HEATER_OFF_CONSUMPTION = 0;

float daily_temperature[] = {-1, -1.8, -3, -4.2, -4.5, -3.5, -2.2, -1, 0.5, 3, 5, 7.2,
8.5, 9.4, 10, 9.6, 8.3, 7.3, 6.1, 5, 3.6, 2.5, 1.3, 0.2};

void initialize_heater()
{   
    if (DEBUG)
        printf("STARTING INITIALIZATION\n");
    time_t timer;
    time(&timer);
    struct tm *tm_struct = localtime(&timer);
    int hour = tm_struct->tm_hour;
    int minute = tm_struct->tm_min;
    daytime_minutes = hour * 60 + minute;
    float percentage = 1.0 - (float)minute / 60.0;
    t_outside = percentage * daily_temperature[hour] + (1.0 - percentage) * daily_temperature[(hour + 1) % 24];
    t_inside = t_outside;
    t_heater = t_outside;
    if (DEBUG)
        printf("INITIALIZATION COMPLETE, t_outside = %f, t_inside = %f and t_heater = %f\n",
        t_outside, t_inside, t_heater);

    return;
}

void update_temperature()
{
    if (DEBUG)
        printf("UPDATING TEMPERATURE\n");
    time_t timer;
    time(&timer);
    struct tm *tm_struct = localtime(&timer);
    int hour = tm_struct->tm_hour;
    int minute = tm_struct->tm_min;
    
    int last_daytime = daytime_minutes;
    daytime_minutes = hour * 60 + minute;
    // Updating t_outside
    float percentage = 1.0 - (float)minute / 60.0;
    t_outside = percentage * daily_temperature[hour] + (1.0 - percentage) * daily_temperature[(hour + 1) % 24];
    // couting the number of minutes between now and the last time we updated the values
    int minutes = 0;
    if (daytime_minutes >= last_daytime){
        minutes = daytime_minutes - last_daytime;
    }else{
        minutes = daytime_minutes + 24*60 - last_daytime;
    }
    // Updating t_heater
    if (heater_state == 0){
        t_heater = max(t_outside, t_heater - COOLING * minutes);
    }else{
        t_heater = min(MAX_HEATER_TEMP, t_heater + HEATING * minutes);
    }
    // Updating t_inside
    t_inside = t_inside/2 + (t_outside + t_heater)/4;

    if (DEBUG)
        printf("UPDATE COMPLETE, t_outside = %f, t_inside = %f and t_heater = %f, state = %i\n",
        t_outside, t_inside, t_heater, heater_state);

    return;
}

float get_temperature(){
    return t_inside;
}

void change_heater_state(int new_state){
    heater_state = new_state;
}

char * get_heater_state(){
    if (heater_state)
        return "ON";
    return "OFF";
}

float get_heater_consumption(){
    if (heater_state)
        return HEATER_ON_CONSUMPTION;
    return HEATER_OFF_CONSUMPTION;
}

void *heaterThread(void *vargp) 
{
    if (DEBUG)
        printf("STARTING THREAD FUNCTION\n");
    while(1){
        if(t_inside <= START_HEATING_AT){
            change_heater_state(1);
        }else if(t_inside >= STOP_HEATING_AT){
            change_heater_state(0);
        }
        update_temperature();
        sleep(3 * 60);
    }
    return 0; 
} 

void run_heater(){
    if (DEBUG)
        printf("STARTING MAIN\n");
    initialize_heater();consumption
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, heaterThread, NULL); 
    pthread_join(thread_id, NULL);
    return;
}

int main_heater(){
    //run_heater();
    return 0;
}