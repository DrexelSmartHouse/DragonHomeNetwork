# DragonHomeNetwork

Short intro

## Contents
- [Quick start](#quickstart)
  - [Necessary hardware](#necessaryhardware)
  - [Necessary tools](#necessarytools)
  - [Hardware setup](#hardwaresetup)
    - [RFM69 Arduino](#rfm69arduino)
    - [Ethernet Arduino](#ethernetarduino)
  - [Network setup](#networksetup)
- [Testing / Quality Assurance](#qa)
- [Troubleshooting](#troubleshooting)
- [Releases](#releases)
- [Contributors](#contributors)

## Quick start <a name="quickstart"></a>
### Necessary hardware <a name="necessaryhardware"></a>
The following hardware is necessary to set up the network and connect one node:
- (3) Arduino Uno
- (2) Arduino Proto Shield with Mini Breadboard
- (1) Arduino Ethernet Shield
- (2) RFM69HCW Wireless Transceiver - 915MHz
- (2) 10Kohm through-hole resistors
- (1) DHT11 Temperature-Humidity sensor
- (3) 9V 1A Arduino power supplies
- (2) voltage/logic converters
- (1) Ethernet cable
- Solid core wires
- Solder

### Necessary tools <a name="necessarytools"></a>
- Soldering iron
- Wire strippers
- Arduino/USB cable
- Internet-connected computer

### Hardware setup <a name="necessarytools"></a>
#### RFM69 Arduino <a name="rfm69arduino"></a>
...

#### Ethernet Arduino <a name="ethernetarduino"></a>
...

### Network setup <a name="networksetup"></a>
Below are the steps to initialize the server, gateway, and one node.
1. **Internet and gateway node setup. (proper title???)**

   Program Arduino firmware (which?) onto a sensor node without a sensor on it (fix wording). Connect this node via txrx serial to another Arduino with Ethernet shield. TODO Include images 288-89, 291-93.

   Load Ethernet-enabled Arduino Uno with uart-mqtt script. This node receives serial information from the gateway and forwards information to the computer. In the uart_mqtt script, the user must enter the IP address of the mqtt server.

   When uploading code onto either board, ensure there are no wires connected between them.

   Program the gateway node with gateway_uart_mqtt script.

   Connect transmit-receive, receive-transmit, common ground wires between the boards.
2. **Server setup.**

    Install mosquitto and mosquitto-clients, then enable mosquitto daemon:

    ```
    sudo apt-get install mosquitto mosquitto-clients
    sudo systemctl enable mosquitto
    ```

    TODO Other software installs

    Clone git repository for rfm69 manager and install dependencies.
    NOTE: shouldn't cloning be unnecessary, since DataCollectionAndAnalysis is a dir in this repository now?
    ```
    git clone drexelsmarthouse/dir/to/data-collection-analysis
    sudo apt-get install python-pip
    sudo pip install paho-mqtt

    ```

    Open a tmux session in the cloned DataCollectionAndAnalysis directory, edit the server config file, and run the script. NOTE: Line number?
    ```
    cd /path/to/DataCollectionAndAnalysis
    vim dshPython.config
      Change server IP to 127.0.0.1
    tmux new -s dsh
    python ./rfm69_mqtt_manager.py
    ```

    Now you can view sweep and scan events, as well as the sensor data and log messages as they are generated. To leave the server running, type `CTRL+B d`. To bring the session back, type `tmux a`.

3. **Sensor node setup**

   Install simple dht library (how?).

   In the dht11_sensor_node script, change the node id to match end node id. NOTE: Line number?

## Testing / Quality Assurance <a name="qa"></a>


## Troubleshooting <a name="troubleshooting"></a>
...

## Releases <a name="releases"></a>
Not currently released.

## Contributors <a name="contributors"></a>
The DragonHome Network team:

- [Joshua Cohen](https://github.com/jcohen98)
- [Reno Farnesi](https://github.com/nfarnesi4)
- [Ryan Hassing](https://github.com/ryanhassing)
- [Timothy Lechman](https://github.com/tlechman49)
