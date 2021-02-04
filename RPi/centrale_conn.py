import sys
sys.path.append(".")
from datetime import datetime
import socket
import _thread
import time
from GarbageBin import GarbageBin
from RealTimeTrafficLight import RealTimeTrafficLight
from SmartStreetLight import SmartStreetLight

HOST = ''
PORT = 2727

CENTRALE_HOST = '192.168.1.176'
CENTRALE_PORT = 65432

CONDITION = [True]

def multi_threaded_client(connection):
    data = connection.recv(1024)
    if not data:
        connection.close()
        return

    message = data.decode('utf-8')
    result = ''

    if message == "exit":
        print("Closing server")
        CONDITION[0] = False
        connection.close()
        return

    path = message.split("/")
    action = path[0]
    sensor = path[1]

    # Get the value of the sensor
    if action == 'get':
        if sensor == "traffic_light":
            result = traffic_light.get_value()
        elif sensor == "street_light":
            result = street_light.get_value()
        elif sensor == "garbage":
            result = garbage.get_value()
    # Get the consumption of the monitored object
    elif action == 'consumption':
        if sensor == "traffic_light":
            result = traffic_light.get_consumption()
        elif sensor == "street_light":
            result = street_light.get_consumption()
        elif sensor == "garbage":
            result = garbage.get_consumption()

    connection.sendall(str.encode(str(result)))
    connection.close()


def update(objects_list):
    while CONDITION[0]:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((CENTRALE_HOST, CENTRALE_PORT))
            # Create the value to be sent to the Centrale node
            # /rpi_update/sensor1:value:state:consumption:date*sensor2:value:state:date
            result = "rpi_update/"
            for object in objects_list:
                date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
                result += object.get_name() + ":" + object.get_value() + ":" + object.get_state()
                result += ":" + object.get_consumption() + ":" + date + "*"
            result = result[:-1]
            s.sendall(bytes(result, 'utf-8'))
            s.close()
        time.sleep(10*60)

if __name__ == "__main__":
    global traffic_light, street_light, garbage
    traffic_light = RealTimeTrafficLight()
    street_light = SmartStreetLight()
    garbage = GarbageBin()
    objects_list = [traffic_light, street_light, garbage]
    # Start a thread to automatically update the values in the DB in Centrale Node
    _thread.start_new_thread(update, (objects_list, ))
    # Multi-thread server that listens to at most 10 clients
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as ServerSideSocket:
        try:
            ServerSideSocket.bind((HOST, PORT))
        except socket.error as e:
            print(str(e))

        print('Socket is listening..')
        ServerSideSocket.listen(3)
        while CONDITION[0]:
            Client, address = ServerSideSocket.accept()
            print('Connected to: ' + address[0] + ':' + str(address[1]))
            _thread.start_new_thread(multi_threaded_client, (Client, ))
        ServerSideSocket.close()