/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) REST Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resources/extern_var.h"
//#include "resources/res-washing-machine.h"
#include "resources/res-Fan.h"
#include "resources/res-stereo.h"
#include "resources/res-TV.h"
//#include "resources/temperature.h"
//#include "resources/lamp.h"
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"

#include "dev/serial-line.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t
  res_hello,
  res_mirror,
  res_chunks,
  res_separate,
  res_push,
  res_event,
  res_sub,
  res_b1_sep_b2,
  res_pressure,
  res_gyros,
  res_accel,
  res_new_alarm,
  //res_value_washing_machine,
  //res_state_washing_machine,
  //res_consumption_washing_machine,
  //res_value_heater,
  //res_state_heater,
  //res_consumption_heater,
  //res_value_lamp,
  //res_state_lamp,
  //res_consumption_lamp,
  res_value_stereo,
  res_state_stereo,
  res_consumption_stereo,
  res_value_TV,
  res_state_TV,
  res_consumption_TV,
  res_value_Fan,
  res_state_Fan,
  res_consumption_Fan,
  res_power_meter,
  res_magne;
  
#if PLATFORM_HAS_LEDS
extern resource_t res_leds, res_toggle;
#endif
#if PLATFORM_HAS_LIGHT
#include "dev/light-sensor.h"
extern resource_t res_light;
#endif
#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"
extern resource_t res_battery;
#endif
#if PLATFORM_HAS_TEMPERATURE
#include "dev/temperature-sensor.h"
extern resource_t res_temperature;
#endif
/*
extern resource_t res_battery;
#endif
#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
extern resource_t res_radio;
#endif
#if PLATFORM_HAS_SHT11
#include "dev/sht11/sht11-sensor.h"
extern resource_t res_sht11;
#endif
*/
#if PLATFORM_HAS_PRESSURE
#include "dev/pressure-sensor.h"
extern resource_t res_pressure;
#endif
#if PLATFORM_HAS_GYROSCOPE
#include "dev/gyr-sensor.h"
extern resource_t res_gyros;
#endif
#if PLATFORM_HAS_ACCELEROMETER
#include "dev/acc-mag-sensor.h"
extern resource_t res_accel;
#endif
#if PLATFORM_HAS_MAGNETOMETER
#include "dev/acc-mag-sensor.h"
extern resource_t res_magne;
#endif

extern char* res_serial_data;

PROCESS(er_example_server, "Erbium Example Server");
//PROCESS(heater, "Heater");
//PROCESS(lamp, "Lamp");
AUTOSTART_PROCESSES(&er_example_server);
/*
// Heater Object
PROCESS_THREAD(heater, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;

  initialize_heater();

  etimer_set(&timer, 3 * 60 * CLOCK_SECOND);
  
  if (DEBUG)
    printf("STARTING HEATER\n");

  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_TIMER) {
        if(t_inside <= START_HEATING_AT){
            change_heater_state(1);
        }else if(t_inside >= STOP_HEATING_AT){
            change_heater_state(0);
        }
        update_temperature();
      etimer_restart(&timer);
    }
  }

  PROCESS_END();
} */
/*---------------------------------------------------------------------------*/
/*
// Lamp Object
PROCESS_THREAD(lamp, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;

  initialize_lamp(1);

  etimer_set(&timer, 3 * 60 * CLOCK_SECOND);
  
  if (DEBUG)
      printf("STARTING LAMP\n");

  while(1){
      if(get_lux(1) <= TURN_ON_AT){
          change_lamp_state(1);
      }
      else if(get_lux(1) >= TURN_OFF_AT){
          change_lamp_state(0);
      }
      update_lux();
      etimer_restart(&timer);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  PRINTF("Starting Erbium Example Server\n");

#ifdef RF_CHANNEL
  PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
  PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

  /* Initialize the REST engine. */
  rest_init_engine();

  /*
   * Bind the resources to their Uri-Path.
   * WARNING: Activating twice only means alternate path, not two instances!
   * All static variables are the same for each URI path.
   */
  rest_activate_resource(&res_hello, "test/hello");
/*  rest_activate_resource(&res_mirror, "debug/mirror"); */
/*  rest_activate_resource(&res_chunks, "test/chunks"); */
/*  rest_activate_resource(&res_separate, "test/separate"); */

  rest_activate_resource(&res_push, "test/push");
  rest_activate_resource(&res_event, "test/serial");
  rest_activate_resource(&res_new_alarm, "my_res/new_alarm");
  rest_activate_resource(&res_value_stereo, "stereo");
  rest_activate_resource(&res_state_stereo, "stereo/state");
  rest_activate_resource(&res_consumption_stereo, "stereo/consumption");
  rest_activate_resource(&res_value_TV, "TV");
  rest_activate_resource(&res_state_TV, "TV/state");
  rest_activate_resource(&res_consumption_TV, "TV/consumption");
  rest_activate_resource(&res_value_Fan, "Fan")
  rest_activate_resource(&res_state_Fan, "Fan/state");
  rest_activate_resource(&res_consumption_Fan, "Fan/consumption");
  rest_activate_resource(&res_power_meter, "power_meter");

  
  //rest_activate_resource(&res_value_washing_machine, "washing_machine");
  //rest_activate_resource(&res_state_washing_machine, "washing_machine/state");
  //rest_activate_resource(&res_consumption_washing_machine, "washing_machine/consumption");
  //rest_activate_resource(&res_consumption_heater, "heater/consumption");
  //rest_activate_resource(&res_state_heater, "heater/state");
  //rest_activate_resource(&res_value_heater, "heater");
  //rest_activate_resource(&res_consumption_lamp, "lamp/consumption");
  //rest_activate_resource(&res_state_lamp, "lamp/state");
  //rest_activate_resource(&res_value_lamp, "lamp");

/*  rest_activate_resource(&res_sub, "test/sub"); */
/*  rest_activate_resource(&res_b1_sep_b2, "test/b1sepb2"); */
#if PLATFORM_HAS_LEDS
  rest_activate_resource(&res_leds, "actuators/leds");
  rest_activate_resource(&res_toggle, "actuators/toggle");
#endif
#if PLATFORM_HAS_LIGHT
  rest_activate_resource(&res_light, "sensors/light"); 
  SENSORS_ACTIVATE(light_sensor);  
#endif
#if PLATFORM_HAS_BATTERY
  rest_activate_resource(&res_battery, "sensors/battery");  
  SENSORS_ACTIVATE(battery_sensor);  
#endif
#if PLATFORM_HAS_TEMPERATURE
  rest_activate_resource(&res_temperature, "sensors/temperature");  
  SENSORS_ACTIVATE(temperature_sensor);  
#endif
/*
#if PLATFORM_HAS_RADIO
  rest_activate_resource(&res_radio, "sensors/radio");  
  SENSORS_ACTIVATE(radio_sensor);  
#endif
#if PLATFORM_HAS_SHT11
  rest_activate_resource(&res_sht11, "sensors/sht11");  
  SENSORS_ACTIVATE(sht11_sensor);  
#endif
*/
#if PLATFORM_HAS_PRESSURE
  rest_activate_resource(&res_pressure, "sensors/pressure");
  SENSORS_ACTIVATE(pressure_sensor);
#endif
#if PLATFORM_HAS_GYROSCOPE
  rest_activate_resource(&res_gyros, "sensors/gyros");
  SENSORS_ACTIVATE(gyr_sensor);
#endif
#if PLATFORM_HAS_ACCELEROMETER
  rest_activate_resource(&res_accel, "sensors/accel");
  SENSORS_ACTIVATE(acc_sensor);
#endif
#if PLATFORM_HAS_MAGNETOMETER
  rest_activate_resource(&res_magne, "sensors/magne");
  SENSORS_ACTIVATE(mag_sensor);
#endif

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == serial_line_event_message) {
      res_serial_data = (char*)data;

      /* Call the event_handler for this application-specific event. */
      res_event.trigger();

      /* Also call the separate response example handler. */
      // res_separate.resume();
    }
  }                             /* while (1) */

  PROCESS_END();
}
