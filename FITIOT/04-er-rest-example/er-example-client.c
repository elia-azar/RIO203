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
 *      Erbium (Er) CoAP client example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include "dev/button-sensor.h"

#include "dev/serial-line.h"
#include "dev/leds.h"

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

/* FIXME: This server address is hard-coded for Cooja and link-local for unconnected border router. */
#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0x2001, 0x660, 0x5307, 0x3127, 0, 0, 0, 0xb468)      /* cooja2 */
/* #define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x1) */

#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)

#define TOGGLE_INTERVAL 10
int toggle_interval = 10;

char last_accel[15];
char new_accel[15];
long high_pressure = 0;

void buffer_clear(char* buffer, int i){
  int l = 0;
  for(l; l<i; l++){
    buffer[l] = 0;
  }
}

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


PROCESS(er_example_client, "Erbium Example Client");
PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server, &er_example_client);

uip_ipaddr_t server_ipaddr;
static struct etimer et;

/* Example URIs that can be queried. */
#define NUMBER_OF_URLS 4
/* leading and ending slashes only for demo purposes, get cropped automatically when setting the Uri-Path */
char *service_urls[NUMBER_OF_URLS] =
{ ".well-known/core", "/sensors/pressure", "/sensors/accel", "/sensors/light" };
#if PLATFORM_HAS_BUTTON
static int uri_switch = 0;
#endif

/* This function is will be passed to COAP_BLOCKING_REQUEST() to handle responses. */
void
client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}

void
client_chunk_handler_light(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  // transforming this string to a long
  long lux = strtol((char *)chunk , NULL, 10);

  // if the leds are turned on, add +10 to the original value, elsewhere do nothing
  long lux_dup = lux;
  if(leds_get()) lux_dup += 10;

  // if it is dark and the leds are turned off, then turn on the leds
  if(lux_dup < 10 && !leds_get()){
    leds_on(LEDS_ALL);
    printf("Turning on the lights\n");
  }// if it is bright and the leds are turned on, then turn off the leds
  else if (lux_dup > 15 && leds_get()){
    leds_off(LEDS_ALL);
    printf("Turning off the lights\n");
  }

  if(leds_get()) lux += 10;

  printf("lux: %i\n", lux);
  printf("LEDS state = %X\n", leds_get());
}

void
client_chunk_handler_accel(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("Accel: %.*s\n", len, (char *)chunk);

  buffer_clear(last_accel, 15);
  strncpy(last_accel, new_accel, 15);
  buffer_clear(new_accel,  15);
  strncpy(new_accel, chunk, strlen(chunk));

  // if accel value has changed, print an Alert
  if (strncmp(last_accel, new_accel, 15) != 0){
    printf("ALERT: Accel value has changed\n");
    printf("last: %s, and new: %s", last_accel, new_accel);
  }
}

void
client_chunk_handler_pressure(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);
  
  long pressure = strtol((char *)chunk , NULL, 10);

  if(pressure >= 1000 && high_pressure != -1){
    printf("Pressure too high, command to fly at a higher altitude\n");
    high_pressure = -1;
  }else if(pressure <= 900 && high_pressure != 1){
    printf("Pressure too low, command to fly at a lower altitude\n");
    high_pressure = 1;
  }else if (pressure > 900 && pressure < 1000){
    high_pressure = 0;
  }

  printf("Pressure: %i\n", pressure + high_pressure*20);

}

PROCESS_THREAD(er_example_client, ev, data)
{
  PROCESS_BEGIN();

  static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

  SERVER_NODE(&server_ipaddr);

  /* receives all CoAP messages */
  coap_init_engine();

  etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);

#if PLATFORM_HAS_BUTTON
  SENSORS_ACTIVATE(button_sensor);
  printf("Press a button to request %s\n", service_urls[uri_switch]);
#endif

  while(1) {
    PROCESS_YIELD();

    if(etimer_expired(&et)) {
      printf("--Toggle timer LIGHT--\n");

      /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      char * service = service_urls[3];
      coap_set_header_uri_path(request, service);

      const char msg[] = "Toggle!";

      coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

      PRINT6ADDR(&server_ipaddr);
      PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

      COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
                            client_chunk_handler_light);

      printf("\n--Done LIGHT--\n");

      printf("--Toggle timer ACCEL--\n");

      /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      service = service_urls[2];
      coap_set_header_uri_path(request, service);

      
      coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

      PRINT6ADDR(&server_ipaddr);
      PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

      COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
                            client_chunk_handler_accel);

      printf("\n--Done ACCEL--\n");

      printf("--Toggle timer PRESSURE--\n");

      /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
      service = service_urls[1];
      coap_set_header_uri_path(request, service);

      coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

      PRINT6ADDR(&server_ipaddr);
      PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

      COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
                            client_chunk_handler_pressure);

      printf("\n--Done PRESSURE--\n");

      etimer_reset(&et);

#if PLATFORM_HAS_BUTTON
    } else if(ev == sensors_event && data == &button_sensor) {

      /* send a request to notify the end of the process */

      coap_init_message(request, COAP_TYPE_CON, COAP_GET, 3);
      coap_set_header_uri_path(request, service_urls[uri_switch]);

      printf("--Requesting %s--\n", service_urls[uri_switch]);

      PRINT6ADDR(&server_ipaddr);
      PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

      COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
                            client_chunk_handler);

      printf("\n--Done--\n");

      uri_switch = (uri_switch + 1) % NUMBER_OF_URLS;
#endif
    }
  }

  PROCESS_END();
}

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
/*  rest_activate_resource(&res_event, "test/serial"); */
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

      printf("recv\n");

      /* Call the event_handler for this application-specific event. */
      res_event.trigger();

      /* Also call the separate response example handler. */
      // res_separate.resume();
    }
  }                             /* while (1) */

  PROCESS_END();
}


