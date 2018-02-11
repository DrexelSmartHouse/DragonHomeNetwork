#!/usr/bin/env python

"""
This is a simple mqtt client that manages the rfm69 network.
It does things like poll the sensors at a specified interval
and check when a device goes down.
"""

import paho.mqtt.client as mqtt
import time
import signal, sys
import datetime
from config_file import get_conf

# conf file file default path
conf_file_path = 'dshPython.conf'
log_file_name = 'rfm69_network.log'
startTime = time.clock()

# callback functions

def on_connect(client, userdata, flags, rc):
    print('connected with result: ' + str(rc))

    # resubscribe whenever connecting or reconnecting
    client.subscribe("RFM69/0/log")

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    spTopic = msg.topic.split("/")
    fields = ('Time', 'Network ID', 'Node ID', 'Sensor Type', 'Sensor Value')
    if(len(spTopic)>4):
        try:
            with open(dataFileName, 'r' ,newline='') as data:
                #file was found, if not, exception
                pass
            with open(dataFileName, 'a' ,newline='') as data:
                writer = csv.DictWriter(data, fields)
                writer.writerow({fields[0]: datetime.datetime.now(),
                                fields[1]: spTopic[2],
                                fields[2]: spTopic[3],
                                fields[3]: spTopic[4],
                                fields[4]: float(msg.payload)})
        except FileNotFoundError:
            print('File was not found, creating "dshData.csv"')
            with open(dataFileName, 'w' ,newline='') as data:
                writer = csv.DictWriter(data, fields)
                headers = dict((n,n) for n in fields )
                writer.writerow(headers)
                writer.writerow({fields[0]: datetime.datetime.now(),
                                 fields[1]: spTopic[2],
                                 fields[2]: spTopic[3],
                                 fields[3]: spTopic[4],
                                 fields[4]: float(msg.payload)})

def main():

    signal.signal(signal.SIGINT, signal_handler)

    # get consts from the config file
    try:
        conf_file = open(conf_file_path, 'r')
        config = get_conf(conf_file)
        conf_file.close()
    except:
        print('Bad config file, ending')
        sys.exit(0)

    #reads config file and returns relevant variables
    #TODO default values in case these are ommited from the config

    server_ip = config['server ip']
    server_port = config['server port']
    client_id = config['client id']
    keep_alive = config['keep alive'] # max number of seconds without sending a message to the broker
    sweep_interval_s = config['interval']

    client = mqtt.Client(client_id)

    # set the callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    # start the connection
    client.connect(server_ip, server_port, keep_alive)

    # start the thread for processing incoming data
    client.loop_start()

    # main loop
    while True:
        time.sleep(10)

    # stop the thread
    client.loop_stop()

if __name__ == '__main__':
    main()
