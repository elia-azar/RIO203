#!/usr/bin/env python3

import socket
import _thread

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
        new_state = ""
        if len(path) > 2:
            new_state = path[2]

        #TODO finish the implementation of this part
        # Get the value of the sensor
        if action == 'get':
            if sensor == "temperature":
                result = 1
            result = 0
        # Change state of a sensor/object
        elif action == 'actuate':
            result = 200
        # Get the consumption of the monitored object
        elif action == 'consumption':
            result = 1

        connection.sendall(str.encode(result))

    connection.close()

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