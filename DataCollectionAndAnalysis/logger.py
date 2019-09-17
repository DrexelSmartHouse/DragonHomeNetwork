#!/usr/bin/env python

"""
This is a simple mqtt logging client
"""

import paho.mqtt.client as mqtt
import context
import logging
import signal
import sys
import time
from config_file import get_conf

# conf file file default path
conf_file_path = 'dshPython.conf'
log_file_name = 'rfm69_network.log'

logging.basicConfig(level=logging.DEBUG)

# callback functions


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

    # reads config file and returns relevant variables
    # TODO default values in case these are ommited from the config

    server_ip = config['server ip']
    server_port = config['server port']
    client_id = config['client id']
    client_id += "_log"

    client = mqtt.Client(client_id)

    logger = logging.getLogger(__name__)
    client.enable_logger(logger)

    # start the connection
    client.connect(server_ip, server_port)
    client.subscribe("DHN/0/log")

    # start the thread for processing incoming data
    client.loop_forever()

    # main loop
    while True:
        time.sleep(10)

    # stop the thread
    client.loop_stop()


if __name__ == '__main__':
    main()
