import sys
sys.path.append(".")
import socket
import _thread
from GarbageBin import GarbageBin
from RealTimeTrafficLight import RealTimeTrafficLight
from SmartStreetLight import SmartStreeLight

HOST = ''
PORT = 2727

CONDITION = [True]

def multi_threaded_client(connection):
    while CONDITION[0]:
        data = connection.recv(1024)
        if not data:
            break

        message = data.decode('utf-8')
        result = ''
    
        if message == "exit":
            print("Closing server")
            CONDITION[0] = False
            break

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

        connection.sendall(str.encode(result))

    connection.close()


if __name__ == "__main__":
    global traffic_light, street_light, garbage
    traffic_light = RealTimeTrafficLight()
    street_light = SmartStreeLight()
    garbage = GarbageBin()
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