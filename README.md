# RIO203 project

This repository contains scripts to emulate a smart city using IoT applications. We have several connected objects with sensors. Their values, consumption and state are collected and sent to the dashboard. For the latter, we use [ThingsBoard](https://thingsboard.io/).

We consider 4 components in this project:
* The Dashboard: A customer can log in and monitor his nodes.
* The Central node: A component that sits in the middle between the dashboard and the nodes. It parses the request, collects the data, stores it in a [DB](https://www.postgresql.org/docs/), and sends it back to the dashboard.
* [FitIoT](https://www.iot-lab.info/testbed/experiment): An IoT platforme where nodes can be reserved and can be reachable via ipv6. It emulates 3 smart homes.
* RPi: Small computer connected to several sensors from the [SunFounderKit](https://www.sunfounder.com/). It emulates smart city applications.


The detailed instructions to set up the experiment are defined below.

## ThingsBoard
Our dashboard is hosted by https://demo.thingsboard.io. For the login, we use the following credentials: Ask the contributors for credentials.


## FitIoT
In order to use the FitIoT platform, you should be registered and have your own credentials. In this case, you can reserve nodes, set up a border-router and flash the er-example-server.iotlab-m3 firmware.
But their is a problem in the FitIoT platform (The nodes are reachable with ipv6. The requests are received by the nodes, but the responses that are sent over the internet are dropped, and therefore, a Time Out error occurs).

So whether you have credentials or not, the following approach is preferred:

Open 3 windows and run the following:

```
home_1# cd RIO203/FITIOT/04-er-rest-example
home_1# make
home_1# sudo ./local-server
```

```
home_2# cd RIO203/FITIOT/04-er-rest-example
home_2# sudo ./local-server-2
```

```
home_3# cd RIO203/FITIOT/04-er-rest-example
home_3# sudo ./local-server-3
```
Now, the 3 smart homes are up and running, and can answer to requests.
It should be noted that **pseudo-sensors** were used due to the encountered problem with FitIoT.

## RPi
Smart city applications are emulated by using a RPi connected to several sensors via a breadboard. The following is a list of the connected objects:

* Traffic light: An infra-red sensor and an RGB led were used.
* Street light: A photoresistor and a dual-core led were used.
* Garbage bin: An ultrasonic sensor was used.

Before running a socket server on the RPi and grant access to the sensors, you should modify the **CENTRALE_HOST** variable in RPi/centrale_conn.py and put the ipv4 address of the machine that is going to run centrale_b.py.

Once the script modified, run the following in order to grant access to the sensors over the internet:

```
rpi# cd RIO203/RPi/
rpi# sudo python3 centrale_conn.py
```

## Central node

### Setup the DB in a docker container (Optional)
For the Database, we use postgresql running in a container.

```
# docker run --name postgres-docker -e POSTGRES_PASSWORD=postgres -p 5432:5432 \
-v postgres-volume:/var/lib/postgresql/data -d postgres

# docker exec -it postgres-docker bash

container# psql -U postgres

psql# CREATE TABLE sensors (
	time DATE UNIQUE NOT NULL,
    home_id INT NOT NULL,
    sensor VARCHAR(20) NOT NULL,
    value real,
    state VARCHAR(50),
    Consumption real,
    PRIMARY KEY (home_id, sensor)
    );

```

### RUN the central node
If you want to run the central node with a DB, the previous step is mandatory. Also, you should uncomment some lines in centrale_b.py to enable the connection between the node and the DB.

Before running the API, you should modify the **RPI_HOST** variable in Centrale/centrale_b.py and put the ipv4 address of the RPi.

Afterwards, just run the following code in the same machine that ran the local-servers code:

```
central# cd RIO203/Centrale
central# python3 centrale_b.py
```

### Enable automatic collection of data, store it, and send it to the dashboard

```
city# cd RIO203/Dashboard
city# python3 city.py
```

```
house_1# cd RIO203/Dashboard
house_1# python3 house_1.py
```

```
house_2# cd RIO203/Dashboard
house_2# python3 house_2.py
```

```
house_3# cd RIO203/Dashboard
house_3# python3 house_3.py
```
