import time
import json
import socket
import paho.mqtt.client as mqtt
import ast
import _thread

CENTRALE_HOST = ''
CENTRALE_PORT = 65432

HOUSE_ID = 3

ACTIONS = ["get", "state", "consumption"]

# keys sensor_id
SENSORS = [
    "garage",
    "dryer",
    "power_meter",
    "refrigerator"
]

# Thingsboard platform credentials
THINGSBOARD_HOST = 'demo.thingsboard.io'

GARAGE_ACCESS_TOKEN = "N8PFRl2uR91Ht3Z6Tt5T"
DRYER_ACCESS_TOKEN = "GkUfBsbTmeSUT3ifILPk"
POWER_METER_ACCESS_TOKEN = "qtTrO2mmLK08svOw7nv1"
REFRIGERATOR_ACCESS_TOKEN = "vNq8XLUY2F2zaIvoi9px"



INTERVAL = 5
sensor_data = {}
next_reading = time.time()

# mqtt client for garage
client_garage = mqtt.Client()
client_garage.username_pw_set(GARAGE_ACCESS_TOKEN)
client_garage.connect(THINGSBOARD_HOST,1883,60)
client_garage.loop_start()
# mqtt client for dryer
client_dryer = mqtt.Client()
client_dryer.username_pw_set(DRYER_ACCESS_TOKEN)
client_dryer.connect(THINGSBOARD_HOST,1883,60)
client_dryer.loop_start()
# mqtt client for refrigerator
client_refrigerator = mqtt.Client()
client_refrigerator.username_pw_set(REFRIGERATOR_ACCESS_TOKEN)
client_refrigerator.connect(THINGSBOARD_HOST,1883,60)
client_refrigerator.loop_start()
# mqtt client for power_meter
client_power_meter = mqtt.Client()
client_power_meter.username_pw_set(POWER_METER_ACCESS_TOKEN)
client_power_meter.connect(THINGSBOARD_HOST,1883,60)
client_power_meter.loop_start()

MQTT_DICT = {
    "garage": client_garage,
    "dryer": client_dryer,
    "refrigerator": client_refrigerator,
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

client_garage.loop_stop()
client_garage.disconnect()
client_dryer.loop_stop()
client_dryer.disconnect()
client_refrigerator.loop_stop()
client_refrigerator.disconnect()
client_power_meter.loop_stop()
client_power_meter.disconnect()