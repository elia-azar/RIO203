import socket
import _thread
import time
from datetime import datetime
import subprocess
from multiping import MultiPing

HOME_ID = 1

HOST = ''
PORT = 2525

CENTRALE_HOST = ''
CENTRALE_PORT = 65432

CONDITION = [True]

SENSORS_IPV6 = {
    "temperature":"a334", 
    "light":"ab34", 
    "washing_machine":"7214", 
    "power_meter":"d5e2"
}

# Variables used to send coap requests
command = "aiocoap-client coap://[2001:660:5307:3127::"
port = "]:5683"

def update():
    while CONDITION[0]:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((CENTRALE_HOST, CENTRALE_PORT))
            # Create the value to be sent to the Centrale node
            # /rpi_update/sensor1:value:state:consumption:date*sensor2:value:state:date
            result = "home_update/" + str(HOME_ID) + "/"
            for sensor in SENSORS_IPV6.keys():
                date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
                ipv6 = SENSORS_IPV6.get(sensor)
                request = command + ipv6 + port

                # Request to get current value
                request += sensor
                value = subprocess.check_output(request, shell=True)

                # Request to get current consumption
                consumption_request = request + "/consumption"
                consumption = subprocess.check_output(consumption_request, shell=True)

                # Request to get current state
                state_request = request + "/state"
                state = subprocess.check_output(state_request, shell=True)

                result += sensor + ":" + value + ":" + state + ":" + consumption + ":" + date + "*"

            result = result[:-1]
            s.sendall(bytes(result, 'utf-8'))
            s.close()

        time.sleep(3*60)

def debug(connection):
    data = connection.recv(1024)
    if not data:
        connection.close()
        return

    message = data.decode('utf-8')

    if message == "exit":
        print("Closing server")
        CONDITION[0] = False
        connection.close()
        return
    
    if message != "debug":
        connection.close()
        return
    
    for ipv6 in SENSORS_IPV6.values():
        # Create a MultiPing object
        ip = "2001:660:5307:3127::" + ipv6
        mp = MultiPing([ip])
        # Send the pings to those addresses
        mp.send()

        # With a 1 second timout, wait for responses (may return sooner if all
        # results are received).
        responses, no_responses = mp.receive(1)
        result = ip

        if responses:
            result += "reachable: %s" % responses

        if no_responses:
            result += "unreachable: %s" % no_responses

        connection.sendall(str.encode(result))
    connection.close()

if __name__ == "__main__":
    # Start a thread to automatically update the values in the DB in Centrale Node
    _thread.start_new_thread(update, ())
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
            _thread.start_new_thread(debug, (Client, ))
        ServerSideSocket.close()