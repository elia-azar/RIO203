import subprocess
import socket
import _thread
import psycopg2
from datetime import datetime
import time

HOST = '' # Server IP address
PORT = 65432       # Server PORT

RPI_HOST = '192.168.0.254'  # RPI IP address
RPI_PORT = 2727             # RPI PORT


HOUSE_1_HOST = '127.0.0.1'  # HOUSE_1 IP address
HOUSE_1_PORT = 8877             # HOUSE_1 PORT

HOME_ADDRESSES = {
    HOUSE_1_HOST: HOUSE_1_PORT
}

CONDITION = [True] # True means the server is up and running

# keys sensor_id
SENSORS_DICT = {
    1:"temperature",
    2:"light",
    3:"battery",
    4:"power_meter",
    5:"washing_machine",
    6:"street_light",
    7:"traffic_light",
    8:"garbage"
}

# keys sensor_id
REV_SENSORS_DICT = {
    "temperature":1,
    "light":2,
    "battery":3,
    "power_meter":4,
    "washing_machine":5,
    "street_light":6,
    "traffic_light":7,
    "garbage":8
}

RPI_ALLOWED_ACTIONS = ['get', 'consumption', 'state']

# function that gets data from all the nodes and stores them in the DB
def update():
    for home_address, home_port in HOME_ADDRESSES.values():
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((home_address, home_port))
            request = ""
            s.sendall(bytes(request, 'utf-8'))
            data = s.recv(1024)

        # Request to get current value
        request += SENSORS_DICT.get(SENSORS_DICT.get(sensor_id))
        value = subprocess.check_output(request, shell=True)

        # Request to get current consumption
        request += "/consumption"
        consumption = subprocess.check_output(request, shell=True)

        # Request to get current state
        request += "/state"
        state = subprocess.check_output(request, shell=True)

        # Update Database
        date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        cur.execute("UPDATE sensors \
        SET value = {}, \
        consumption = {} \
        timestamp = {}, \
        WHERE home_id = {} AND sensor_id = {};".format(value, consumption, date, home_id, sensor_id))
    return

# update() wrapper called every 10 minutes
def update_10_min():
    while CONDITION[0]:
        update()
        time.sleep(10*60)
    return


# Connect to postgres DB
conn = psycopg2.connect("dbname='postgres' user='postgres' host='127.0.0.1' password='postgres' port='5432'")

# Open a cursor to perform database operations
cur = conn.cursor()

# RPi request handler
#TODO Add state handler
def handle_rpi(action, sensor_id):
    home_id = 0
    result = ""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((RPI_HOST, RPI_PORT))

        # Get the value of the sensor by sending a request
        if action == 'get':
            # Build the request and send it, then receive the response
            request = "get/" + SENSORS_DICT.get(sensor_id)
            s.sendall(bytes(request, 'utf-8'))
            result = repr(s.recv(512).decode('utf-8'))

            # Updating value in the database
            date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            cur.execute("UPDATE sensors \
            SET value = {}, \
            time = {} \
            WHERE home_id = {} AND sensor_id = {};".format(result, date, home_id, sensor_id))
            result = [result, date]
        
        # Get the consumption of a specific sensor
        elif action == 'consumption':
            # Build the request and send it, then receive the response
            request = "/consumption/" + SENSORS_DICT.get(sensor_id)
            s.sendall(bytes(request, 'utf-8'))
            result = repr(s.recv(512).decode('utf-8'))

            # Updating consumption in the database
            date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            cur.execute("UPDATE sensors \
            SET consumption = {}, \
            time = {} \
            WHERE home_id = {} AND sensor_id = {};".format(result, date, home_id, sensor_id))
            result = [result, date]
        s.close()
    return result

# Function that takes a list of objects with their values and updates the DB
def update_db(obj_list, home_id):
    for obj in obj_list:
        components = obj.split(":")
        sensor_id = REV_SENSORS_DICT.get(components[0])
        value = float(components[1])
        state = components[2]
        consumption = components[3]
        date = components[4]
        cur.execute("UPDATE sensors \
            SET consumption = {}, \
            time = {}, \
            value = {}, \
            state = {}, \
            WHERE home_id = {} AND sensor_id = {};".format(consumption, date, value, state, home_id, sensor_id))
    return

# RPi update handler
def rpi_update_handler(connection):
    data = connection.recv(2048)
    if not data:
        connection.close()
        return
    
    message = data.decode('utf-8')

    # Parse the update message and update the DB
    # /rpi_update/sensor1:value:state:consumption:date*sensor2:value:state:...
    path = message.split("/")
    action = path[0]
    home_id = 0
    if action != "rpi_update":
        connection.close()
        return
    objects = path[1].split("*")
    # update the DB
    update_db(objects, home_id)
    # close the connection with the client
    connection.close()

#TODO modify and adapt it to the local-server
# home update handler
def home_update_handler(message):
    # Parse the update message and update the DB
    # /home_update/home_id/sensor1:value:state:consumption:date*sensor2:value:state:date
    path = message.split("/")
    home_id = path[1]
    objects = path[2].split("*")
    # update the DB
    update_db(objects, home_id)

#TODO adapt the requests and send them to the appropriate Home_addr
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
    # message type: action/home_id/sensor_id/new_state
    # authorized actions: get, get_from_db, getall, actuate, consumption, consumption_from_db, state, state_from_db
    # home_id: int
    # sensor_id: int <-> used as a key to get the sensor used (for ex., temperature)
    # new_state: (Optional) str <-> only used if the chosen action is actuate.
    path = message.split("/")
    action = path[0]
    home_id = 100
    sensor_id = 100
    if len(path) > 2:
        home_id = int(path[1])
        sensor_id = int(path[2])
    
    # If the action is home_update, send it to home_update_handler
    if action == "home_update":
        home_update_handler(message)
        connection.close()
        return

    # If home_id == 0, this request is sent to the RPi if the action matches the list
    if home_id == 0 and action in RPI_ALLOWED_ACTIONS:
        result = handle_rpi(action, sensor_id)
        connection.sendall(str.encode(result))
        connection.close()
        return

    # Get the value of the sensor by sending a request
    if action == 'get':
        # Build the request and send it, then receive the response
        request += SENSORS_DICT.get(sensor_id)
        result = subprocess.check_output(request, shell=True)

        # Updating value in the database
        date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        cur.execute("UPDATE sensors \
        SET value = {}, \
        time = {} \
        WHERE home_id = {} AND sensor_id = {};".format(result, date, home_id, sensor_id))
        result = [result, date]

    # Get the consumption of a specific sensor
    elif action == 'consumption':
        # Build the request and send it, then receive the response
        request += SENSORS_DICT.get(sensor_id) + "/consumption"
        result = subprocess.check_output(request, shell=True)

        # Updating consumption in the database
        date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        cur.execute("UPDATE sensors \
        SET consumption = {}, \
        time = {} \
        WHERE home_id = {} AND sensor_id = {};".format(result, date, home_id, sensor_id))
        result = [result, date]
    
    # Get the consumption of a specific sensor
    elif action == 'state':
        # Build the request and send it, then receive the response
        request += SENSORS_DICT.get(sensor_id) + "/state"
        result = subprocess.check_output(request, shell=True)

        # Updating state in the database
        date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        cur.execute("UPDATE sensors \
        SET state = {}, \
        time = {} \
        WHERE home_id = {} AND sensor_id = {};".format(result, date, home_id, sensor_id))
        result = [result, date]

    connection.sendall(str.encode(str(result)))
    connection.close()

# Run a thread that will keep the Database up-to-date
#_thread.start_new_thread(update_10_min, ())

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
        if address[0] != RPI_HOST:
            _thread.start_new_thread(multi_threaded_client, (Client, ))
        else:
            _thread.start_new_thread(rpi_update_handler, (Client, ))
    ServerSideSocket.close()