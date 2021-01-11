import os
import time
import sys
import json
import random
import paho.mqtt.client as mqtt

# Function to read sensor values
def read_from_sensor():
    temp = random.randint(25,45)
    hum = random.randint(50,60)
    air = random.randint(55,60)
    light = random.randint(100,180)
    return temp, hum, air,light
# Thingsboard platform credentials
THINGSBOARD_HOST = 'demo.thingsboard.io'
ACCESS_TOKEN = 'W6cHmJDUHngrSJhRE75h'

INTERVAL = 5
sensor_data = {'temperature' :0,'humidity':0,'air_quality':0,'light_intensity':0}
next_reading = time.time()
client = mqtt.Client()
client.username_pw_set(ACCESS_TOKEN)
client.connect(THINGSBOARD_HOST,1883,60)

client.loop_start()

try:
    while True:
        temp,hum,air,light = read_from_sensor()
        print("Temperature:",temp, chr(176) + "C")
        print("Humidity:", hum,"%rH")
        print("Air Quality:", air,"%")
        print("Light Intensity:",   light,"lux")
        sensor_data['temperature'] = temp
        sensor_data['humidity'] = hum
        sensor_data['air_quality'] = air
        sensor_data['light_intensity'] = light

        client.publish('v1/devices/me/telemetry',json.dumps(sensor_data),1)
        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time >0:
            time.sleep(sleep_time)

except KeyboardInterrupt:
    pass

client.loop_stop()
client.disconnect()