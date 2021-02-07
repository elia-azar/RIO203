import socket
import _thread
#import psycopg2
from datetime import datetime

HOST = '' # Server IP address
PORT = 65432       # Server PORT

RPI_HOST = '192.168.0.254'  # RPI IP address
RPI_PORT = 2727             # RPI PORT


HOUSE_1_HOST = '127.0.0.1'  # HOUSE_1 IP address
HOUSE_1_PORT = 8877             # HOUSE_1 PORT

HOUSE_2_HOST = '127.0.0.1'  # HOUSE_1 IP address
HOUSE_2_PORT = 8888             # HOUSE_1 PORT

HOUSE_3_HOST = '127.0.0.1'  # HOUSE_1 IP address
HOUSE_3_PORT = 8899            # HOUSE_1 PORT

HOME_ADDRESSES = {
    HOUSE_1_HOST: HOUSE_1_PORT
}

CONDITION = [True] # True means the server is up and running

RPI_ALLOWED_ACTIONS = ['get', 'consumption', 'state']


# Connect to postgres DB
#conn = psycopg2.connect("dbname='postgres' user='postgres' host='127.0.0.1' password='postgres' port='5432'")

# Open a cursor to perform database operations
# cur = conn.cursor()

# Function that takes a list of objects with their values and updates the DB
def update_db(obj_list, home_id):
    for obj in obj_list:
        components = obj.split(":")
        sensor = components[0]
        value = float(components[1])
        state = components[2]
        consumption = components[3]
        date = components[4]
        """cur.execute("UPDATE sensors \
            SET consumption = {}, \
            time = {}, \
            value = {}, \
            state = {}, \
            WHERE home_id = {} AND sensor = {};".format(consumption, date, value, state, home_id, sensor))"""
    return


# home update handler
def update_handler(message):
    # Parse the update message and update the DB
    # /update/home_id/sensor1:value:state:consumption:date*sensor2:value:state:...
    path = message.split("/")
    home_id = path[1]
    objects = path[2].split("*")
    # update the DB
    update_db(objects, home_id)

# Client handler
def multi_threaded_client(connection):
    data = connection.recv(2048)
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

    # Parse request
    # message type: action/home_id/sensor/new_state
    # authorized actions: get, consumption, state
    # home_id: int
    # sensor: string <-> used as a key to get the sensor used (for ex., temperature)
    path = message.split("/")
    action = path[0]
    home_id = int(path[1])
    sensor = path[2]
    
    # If the action is update, send it to update_handler
    if action == "update":
        update_handler(message)
        connection.close()
        return

    # Create connexion with local server, and based on the result return the value
    host = 0
    port = 0
    if home_id == 0:
        host = RPI_HOST
        port = RPI_PORT
    elif home_id == 1:
        host = HOUSE_1_HOST
        port = HOUSE_1_PORT
    elif home_id == 2:
        host = HOUSE_2_HOST
        port = HOUSE_2_PORT
    elif home_id == 3:
        host = HOUSE_3_HOST
        port = HOUSE_3_PORT
    else:
        connection.close()
        return

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))

        # Get the value of the sensor by sending a request
        if action == 'get':
            # Build the request and send it, then receive the response
            request = 'get/' + sensor
            s.sendall(bytes(request,'utf-8'))
            result = float(s.recv(512).decode('utf-8'))

            # Updating value in the database
            date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            """cur.execute("UPDATE sensors \
            SET value = {}, \
            time = {} \
            WHERE home_id = {} AND sensor = {};".format(result, date, home_id, sensor))"""
            result = [result, date]

        # Get the consumption of a specific sensor
        elif action == 'consumption':
            # Build the request and send it, then receive the response
            request =  "consumption/" + sensor
            s.sendall(bytes(request,'utf-8'))
            result = float(s.recv(512).decode('utf-8'))

            # Updating consumption in the database
            date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            """cur.execute("UPDATE sensors \
            SET consumption = {}, \
            time = {} \
            WHERE home_id = {} AND sensor = {};".format(result, date, home_id, sensor))"""
            result = [result, date]
        
        # Get the consumption of a specific sensor
        elif action == 'state':
            # Build the request and send it, then receive the response
            request = "state/" + sensor
            s.sendall(bytes(request,'utf-8'))
            result = s.recv(512).decode('utf-8')

            # Updating state in the database
            date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            """cur.execute("UPDATE sensors \
            SET state = {}, \
            time = {} \
            WHERE home_id = {} AND sensor = {};".format(result, date, home_id, sensor))"""
            result = [result, date]

    connection.sendall(str.encode(str(result)))
    connection.close()

# Multi-thread server that listens to at most 10 clients
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as ServerSideSocket:
    try:
        ServerSideSocket.bind((HOST, PORT))
    except socket.error as e:
        print(str(e))

    print('Socket is listening..')
    ServerSideSocket.listen(10)
    while CONDITION[0]:
        Client, address = ServerSideSocket.accept()
        print('Connected to: ' + address[0] + ':' + str(address[1]))
        _thread.start_new_thread(multi_threaded_client, (Client, ))
    ServerSideSocket.close()