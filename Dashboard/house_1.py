import time
import json
import socket
import paho.mqtt.client as mqtt
import ast
import _thread

CENTRALE_HOST = ''
CENTRALE_PORT = 65432

HOUSE_ID = 1

ACTIONS = ["get", "state", "consumption"]

# keys sensor_id
SENSORS = [
    "heater",
    "lamp",
    "power_meter",
    "washing_machine"
]

# Thingsboard platform credentials
THINGSBOARD_HOST = 'demo.thingsboard.io'

LAMP_ACCESS_TOKEN = "dL4OwyYreAwAHawkrM64"
HEATER_ACCESS_TOKEN = "GkUfBsbTmeSUT3ifILPk"
POWER_METER_ACCESS_TOKEN = "wZ2pgb16khBiOeS3ajHJ"
WASHING_MACHINE_ACCESS_TOKEN = "djaHtlZc40iDzisv9mCC"



INTERVAL = 20
sensor_data = {}
next_reading = time.time()

# mqtt client for lamp
client_lamp = mqtt.Client()
client_lamp.username_pw_set(LAMP_ACCESS_TOKEN)
client_lamp.connect(THINGSBOARD_HOST,1883,60)
client_lamp.loop_start()
# mqtt client for heater
client_heater = mqtt.Client()
client_heater.username_pw_set(HEATER_ACCESS_TOKEN)
client_heater.connect(THINGSBOARD_HOST,1883,60)
client_heater.loop_start()
# mqtt client for washing_machine
client_washing_machine = mqtt.Client()
client_washing_machine.username_pw_set(WASHING_MACHINE_ACCESS_TOKEN)
client_washing_machine.connect(THINGSBOARD_HOST,1883,60)
client_washing_machine.loop_start()
# mqtt client for power_meter
client_power_meter = mqtt.Client()
client_power_meter.username_pw_set(POWER_METER_ACCESS_TOKEN)
client_power_meter.connect(THINGSBOARD_HOST,1883,60)
client_power_meter.loop_start()

MQTT_DICT = {
    "lamp": client_lamp,
    "heater": client_heater,
    "washing_machine": client_washing_machine,
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
        data = s.recv(512).decode('utf-8')

        # Parse result
        response = ast.literal_eval(data)
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
        print(action + "/" + sensor + ": " + str(sensor_data))
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

client_lamp.loop_stop()
client_lamp.disconnect()
client_heater.loop_stop()
client_heater.disconnect()
client_washing_machine.loop_stop()
client_washing_machine.disconnect()
client_power_meter.loop_stop()
client_power_meter.disconnect()