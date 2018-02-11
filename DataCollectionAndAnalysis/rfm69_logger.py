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
    log_message = str(datetime.datetime.now()) + ', ' + str(msg.payload) + '\n'
    print(log_message)
    try:
        with open(log_file_name, 'r') as data:
            #file was found, if not, exception
            pass
        with open(log_file_name, 'a') as data:
            data.write(log_message)

    except FileNotFoundError:
        print('Log file was not found, creating' + log_file_name)
        with open(log_file_name, 'w') as data:
            data.write(log_message)

# signal handler for ctrl-c
def signal_handler(signal, frame):
    print("Ending Program.....")
    sys.exit(0)

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
    client_id += "_log"
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
