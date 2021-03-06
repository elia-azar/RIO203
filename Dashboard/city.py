import time
import json
import socket
import paho.mqtt.client as mqtt
import ast
import _thread

CENTRALE_HOST = ''
CENTRALE_PORT = 65432

HOUSE_ID = 0

ACTIONS = ["get", "state", "consumption"]

# keys sensor_id
SENSORS = [
    "street_light",
    "traffic_light",
    "garbage"
]

# Thingsboard platform credentials
THINGSBOARD_HOST = 'demo.thingsboard.io'

STREET_LIGHT_ACCESS_TOKEN = "EYNekz4V1n5EywgBsph1"
GARBAGE_ACCESS_TOKEN = "mHGWTF8CzNkxDAJD4UWN"
TRAFFIC_LIGHT_ACCESS_TOKEN = "dlUlNG4oeYSCDigfusLd"




INTERVAL = 60
sensor_data = {}
next_reading = time.time()

# mqtt client for street_light
client_street_light = mqtt.Client()
client_street_light.username_pw_set(STREET_LIGHT_ACCESS_TOKEN)
client_street_light.connect(THINGSBOARD_HOST,1883,60)
client_street_light.loop_start()
# mqtt client for garbage
client_garbage = mqtt.Client()
client_garbage.username_pw_set(GARBAGE_ACCESS_TOKEN)
client_garbage.connect(THINGSBOARD_HOST,1883,60)
client_garbage.loop_start()
# mqtt client for traffic_light
client_traffic_light = mqtt.Client()
client_traffic_light.username_pw_set(TRAFFIC_LIGHT_ACCESS_TOKEN)
client_traffic_light.connect(THINGSBOARD_HOST,1883,60)
client_traffic_light.loop_start()

MQTT_DICT = {
    "street_light": client_street_light,
    "garbage": client_garbage,
    "traffic_light": client_traffic_light
}

# Function to read sensor values, get state or power consumption
def get(action, sensor):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Send request to the centrale node
        request = "{}/{}/{}".format(action, HOUSE_ID, sensor)
        s.connect((CENTRALE_HOST, CENTRALE_PORT))
        s.sendall(bytes(request, 'utf-8'))
        
        print(request)
        # Receive answer from the centrale node
        data = s.recv(512).decode('utf-8')
        print(data)

        # Parse result
        response = ast.literal_eval(data)
        print('done')
        value = response[0]
        date = response[1]
    
    return value, date

def run(sensor):
    for action in ACTIONS:
        value, date = get(action, sensor)
        sensor_data  = {
            action+" "+sensor: value,
            # "date": date
        }
        MQTT_DICT.get(sensor).publish('v1/devices/me/telemetry',json.dumps(sensor_data),1)
    return

try:
    while True:
        for sensor in SENSORS:
            _thread.start_new_thread(run, (sensor,))

        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)

except KeyboardInterrupt:
    pass

client_street_light.loop_stop()
client_street_light.disconnect()
client_garbage.loop_stop()
client_garbage.disconnect()
client_traffic_light.loop_stop()
client_traffic_light.disconnect()
