#!/usr/bin/env python
"""
This is a python script that handles and store message received by Gateway.
"""
import sys, os, serial, signal, threading, time, json
from datetime import datetime

# callback functions
def monitor(gateway_ser):
   data_point={'0':0,'1':0}
   point_set=set()
   # loop to read incoming data unless terminated
   while (1):
        # read in incoming message in serial port
        line = gateway_ser.readline()
        line_str = str(line)

        # Check if any data is read
        if (line_str != ""):
           #print line[:-1]         # strip \n
            fields = line_str[2:-3] .split('/')
            # Check if the data received is sensors data
            if len(fields) > 1: 
                server_ID = fields[1]
                sensor_ID = fields[2]
                data_point[sensor_ID] = data_point[sensor_ID] + 1
                point_set.add(data_point[sensor_ID])
                data_point[sensor_ID] = len(point_set) 
                # generate string of data 

                # get current time at the time of receiving data
                now = datetime. now()
                cur_time = now. strftime("%D %H:%M:%S")

                # get run number
                with open(os.getcwd() + "/run_config.json") as json_file:
                    data = json.load(json_file)
                    run_num = data['run_num']
                msg = "DataPoint " + str(data_point[sensor_ID]) + " Time "\
                    + cur_time + " Run " + str(run_num) + " SensorID " + sensor_ID
                print("device ID: ", sensor_ID)
                text_file = open(os.getcwd() + "/gateway_data.txt", "a")

                # read in all measurements sent from the sensor
                for idx in range(3,len(fields),2):
                    para = fields[idx]
                    measure = fields[idx+1]     
                    msg = msg + " " + para + " " + measure

                text_file.write(msg + "\n")
                text_file.close()

       # do some other things here

   print("Stop Monitoring")

# handling json file by reading then writing data
def update_json():
    with open(os.getcwd() + "/run_config.json") as json_file:
        data = json.load(json_file)
        run_num = data['run_num']
        print("Run number %d ended" %run_num)
        run_num = run_num + 1
        data['run_num'] = run_num

    with open(os.getcwd() + "/run_config.json", "w") as json_file:   
        json.dump(data, json_file)

# signal handler for ctrl-c    
def signal_handler(signal, frame):
    print("Ending Program.....")
    update_json()
    sys.exit(0)

""" -------------------------------------------
MAIN APPLICATION
"""  
def main():
    # exit gracefully if sigint is received
    signal.signal(signal.SIGINT, signal_handler)

    # setup the serial port based on the config file/or assignment
    gateway_ser = serial.Serial()
    gateway_ser.baudrate = 9600 #config['baudrate']
    gateway_ser.timeout = None  # set blocking

    # check to see if the user inputed a serial port
    if len(sys.argv) == 2:
        gateway_ser.port = sys.argv[1]
    else:
        gateway_ser.port = 'COM6' #config['serial port']

    # attempt to open the serial port
    try:
        gateway_ser.open()
    except serial.SerialException:
        print('could not find serial port: ' + gateway_ser.port)
        sys.exit(1)

    print("Start Serial Monitor\n")

    # Function call to log data coming to serial port
    monitor(gateway_ser)

if __name__ == '__main__':
    main()