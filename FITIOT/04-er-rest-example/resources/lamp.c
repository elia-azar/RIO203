#include <stdio.h>
#include "lamp.h"
#include "common.h"
#include <sys/time.h>
//#include <pthread.h> 
#include <unistd.h>

#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/leds.h"

float lux = 0;
int lamp_state = 0;

const float TURN_ON_AT = 50;
const float TURN_OFF_AT = 60;

const float LAMP_ON_CONSUMPTION = 50;
const float LAMP_OFF_CONSUMPTION = 0;

float daily_light[] = {0.1, 0.1, 0.2, 0.7, 2, 10, 50, 80, 107, 200, 300, 500,
700, 750, 690, 580, 500, 450, 340, 250, 50, 10, 1, 0.5};


/* Light sensor */
static void config_lamp()
{
  light_sensor.configure(LIGHT_SENSOR_SOURCE, ISL29020_LIGHT__AMBIENT);
  light_sensor.configure(LIGHT_SENSOR_RESOLUTION, ISL29020_RESOLUTION__16bit);
  light_sensor.configure(LIGHT_SENSOR_RANGE, ISL29020_RANGE__1000lux);
  SENSORS_ACTIVATE(light_sensor);
  return;
}

static float get_light(){
  int light_val = light_sensor.value(0);
  float light = ((float)light_val / LIGHT_SENSOR_VALUE_SCALE);
  return light;
}

/*---------------------------------------------------------------------------*/

void initialize_lamp(int simulation)
{   
    if (DEBUG)
        printf("STARTING INITIALIZATION\n");
    
    if (simulation){
        time_t timer;
        time(&timer);
        struct tm *tm_struct = localtime(&timer);
        int hour = tm_struct->tm_hour;
        int minute = tm_struct->tm_min;
        float percentage = 1.0 - (float)minute / 60.0;
        lux = percentage * daily_light[hour] + (1.0 - percentage) * daily_light[(hour + 1) % 24];
    }else{
        config_lamp();
    }
    if (DEBUG)
        printf("INITIALIZATION COMPLETE, lux = %f\n", lux);

    return;
}

void update_lux()
{
    if (DEBUG)
        printf("UPDATING light\n");
    time_t timer;
    time(&timer);
    struct tm *tm_struct = localtime(&timer);
    int hour = tm_struct->tm_hour;
    int minute = tm_struct->tm_min;

    // Updating lux
    float percentage = 1.0 - (float)minute / 60.0;
    lux = percentage * daily_light[hour] + (1.0 - percentage) * daily_light[(hour + 1) % 24];

    if (DEBUG)
        printf("UPDATE COMPLETE, lux = %f\n", lux);

    return;
}

float get_lux(int simulation){
    if (simulation){
        return lux;
    }else{
        return get_light();
    }
}

void change_lamp_state(int new_state){
    lamp_state = new_state;
    if (lamp_state){
        leds_on(LEDS_ALL);
    }else{
        leds_off(LEDS_ALL);
    }
}

char * get_lamp_state(){
    if (lamp_state){
        return "ON";
    }
    return "OFF";
}

float get_lamp_consumption(){
    if (lamp_state)
        return LAMP_ON_CONSUMPTION;
    return LAMP_OFF_CONSUMPTION;
}

void *lampThread(void *vargp) 
{
    if (DEBUG)
        printf("STARTING THREAD FUNCTION\n");
    while(1){
        if(get_lux(1) <= TURN_ON_AT){
            change_lamp_state(1);
        }
        else if(get_lux(1) >= TURN_OFF_AT){
            change_lamp_state(0);
        }
        update_lux();
        sleep(3 * 60);
    }
    return 0; 
} 

void run_lamp(){/*
    initialize_lamp();
    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, lampThread, NULL); 
    pthread_join(thread_id, NULL); */
    return;
}


int main_lamp(){
    if (DEBUG)
        printf("STARTING MAIN\n");
    //run_lamp();
    return 0;
}