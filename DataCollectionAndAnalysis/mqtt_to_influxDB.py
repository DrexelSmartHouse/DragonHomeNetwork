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
    print(msg.topic + " " + str(msg.payload))
    spTopic = msg.topic.split("/")
    msg_json = [
        {
            "measurement": "Grafana",
            "tags": {
                "NodeID": spTopic[3],
                "SensorType": spTopic[4]
            },
            "fields": {
                "NetworkID": spTopic[2],
                "SensorValue": float(msg.payload)
            }
        }
    ]
    userdata['influx client'].write_points(msg_json)
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


if __name__ == '__main__':
    main()
