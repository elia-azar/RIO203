import time
import json
import socket
import paho.mqtt.client as mqtt
import ast
import _thread

CENTRALE_HOST = ''
CENTRALE_PORT = 65432

HOUSE_ID = 2

ACTIONS = ["get", "state", "consumption"]

# keys sensor_id
SENSORS = [
    "fan",
    "tv",
    "power_meter",
    "stereo"
]

# Thingsboard platform credentials
THINGSBOARD_HOST = 'demo.thingsboard.io'

FAN_ACCESS_TOKEN = "N8PFRl2uR91Ht3Z6Tt5T"
TV_ACCESS_TOKEN = "GkUfBsbTmeSUT3ifILPk"
POWER_METER_ACCESS_TOKEN = "qtTrO2mmLK08svOw7nv1"
STEREO_ACCESS_TOKEN = "vNq8XLUY2F2zaIvoi9px"



INTERVAL = 5
sensor_data = {}
next_reading = time.time()

# mqtt client for fan
client_fan = mqtt.Client()
client_fan.username_pw_set(FAN_ACCESS_TOKEN)
client_fan.connect(THINGSBOARD_HOST,1883,60)
client_fan.loop_start()
# mqtt client for tv
client_tv = mqtt.Client()
client_tv.username_pw_set(TV_ACCESS_TOKEN)
client_tv.connect(THINGSBOARD_HOST,1883,60)
client_tv.loop_start()
# mqtt client for stereo
client_stereo = mqtt.Client()
client_stereo.username_pw_set(STEREO_ACCESS_TOKEN)
client_stereo.connect(THINGSBOARD_HOST,1883,60)
client_stereo.loop_start()
# mqtt client for power_meter
client_power_meter = mqtt.Client()
client_power_meter.username_pw_set(POWER_METER_ACCESS_TOKEN)
client_power_meter.connect(THINGSBOARD_HOST,1883,60)
client_power_meter.loop_start()

MQTT_DICT = {
    "fan": client_fan,
    "tv": client_tv,
    "stereo": client_stereo,
    "power_meter": client_power_meter
}

# Function to read sensor values, get state or power consumption
def get(action, sensor):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # Send request to the centrale node
        request = "{}/{}/{}".format(action, HOUSE_ID, sensor)
        s.connect((CENTRALE_HOST, CENTRALE_PORT))
        s.sendall(bytes(request, 'utf-8'))
        
        # Receive answer from the centrale node
        data = repr(s.recv(512).decode('utf-8'))

        # Parse result
        response = ast.literal_eval(data)
        value = response[0]
        date = response[1]
    
    return value, date

def run(sensor):
    for action in ACTIONS:
        value, date = get(action, sensor)
        sensor_data  = {
            action: value,
            "date": date
        }
        MQTT_DICT.get(sensor).publish('v1/devices/me/telemetry',json.dumps(sensor_data),1)
    return

try:
    while True:
        for sensor in SENSORS:
            _thread.start_new_thread(run, (sensor))

        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)

except KeyboardInterrupt:
    pass

client_fan.loop_stop()
client_fan.disconnect()
client_tv.loop_stop()
client_tv.disconnect()
client_stereo.loop_stop()
client_stereo.disconnect()
client_power_meter.loop_stop()
client_power_meter.disconnect()