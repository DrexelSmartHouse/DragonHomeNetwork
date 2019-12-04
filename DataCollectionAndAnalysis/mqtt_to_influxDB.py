#!/usr/bin/env python

"""
Please configure your database information.

This information has been redacted from the file due to privacy concerns.

The MQTT message will be parsed into an array with indices in this format:
('Time', 'Network ID', 'Node ID', 'Sensor Type', 'Sensor Value')
"""

import paho.mqtt.client as mqtt
import serial
import io
import signal
import sys
import time
from datetime import datetime
import json

from influxdb import InfluxDBClient

from config_file import get_conf

# conf file file default path
conf_file_path = 'serial-to-mqtt.conf'

# callback functions


def on_connect(client, userdata, flags, rc):
    print('connected with result: ' + str(rc))

    # resubscribe whenever connecting or reconnecting
    client.subscribe("DHN/" + str(userdata['network id']) + "/#")


def on_message(client, userdata, msg):
    now = datetime. now()
    current_time = now. strftime("%D %H:%M:%S")
    print(current_time + " " + msg.topic + " " + str(msg.payload))
    spTopic = msg.topic.split("/")

    # print(current_time)
    msg_json = [
        {
            "measurement": "Grafana",
            "tags": {
                "NodeID": spTopic[2],
                "SensorType": spTopic[3],
                "Time": current_time
            },
            "fields": {
                "NetworkID": spTopic[1],
                "SensorValue": float(msg.payload)
            }
        }
    ]
    # with open('data.txt', 'a') as outfile:
    #     json.dump(msg_json, outfile)
        
    print(userdata['influx client'].write_points(msg_json))

# signal handler for ctrl-c
def signal_handler(signal, frame):
    print("Ending Program.....")
    sys.exit(0)


def main():

    # exit gracefully if sigint is received
    signal.signal(signal.SIGINT, signal_handler)

    # get consts from the config file
    with open(conf_file_path, 'r') as conf_file:
        config = get_conf(conf_file)
        conf_file.close()
    
    # setup the serial port based on the config file
    gateway_ser = serial.Serial()
    gateway_ser.baudrate = config['baudrate']
    gateway_ser.timeout = None  # set blocking

    # check to see if the user inputed a serial port
    if len(sys.argv) == 2:
        gateway_ser.port = sys.argv[1]
    else:
        gateway_ser.port = config['serial port']

    # attempt to open the serial port
    try:
        gateway_ser.open()
    except serial.SerialException:
        print('could not find serial port: ' + gateway_ser.port)
        sys.exit(1)

    # MQTT Setup
    server_ip = config['server ip']
    server_port = config['server port']
    client_id = config['client id']

    # Influx DB Setup
    hostDB = config['host']
    portDB = config['port']
    userDB = config['user']
    pwDB = config['password']
    dbName = config['databasename']

    client = mqtt.Client(client_id)
    clientdb = InfluxDBClient(hostDB, portDB, userDB, pwDB, dbName)
    clientdb.create_database(dbName)
    clientdb.create_retention_policy('great_policy', '1d',1,default=True)
    # set the user data to the open and working serial port
    userdata = dict()
    userdata['network id'] = 0  # TODO get from the gateway
    userdata['influx client'] = clientdb
    client.user_data_set(userdata)

    # set the callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    # start the connection
    client.connect(server_ip, server_port)

    # start the thread for processing incoming data
    client.loop_start()
    
    # TODO get the network ID from the gateway
    network_id = 0

    pub_topic_prefix = "DHN/" + str(network_id)
    # main loop
    while True:

        # block waiting for a message from the gateway
        topic = gateway_ser.readline()
        topic = topic.decode()

        # clean up the message
        # print(message)
        topic = topic.strip()
        topic = topic.split('/')

 
        payload = topic.pop() 
        # payload = message.pop() + ": " + payload
        # print(payload)
        try:
            client.publish('/'.join(topic), payload,1)
        except:
            print(topic)
            print(payload)

    # stop the thread
    client.loop_stop()

if __name__ == '__main__':
    main()