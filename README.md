# DragonHomeNetwork

The exponential growth of wireless communications markets around the world has allowed users from all corners of the globe to share data almost instantaneously. However, managing the collection and transmission of data from multiple devices connected in small, confined networks presents major challenges. The futuristic ideal of having millions of interconnected "things" (inanimate objects, devices, communities, and environments) that have the ability to sense, communicate, network, and produce mass amounts of data that can be utilized in a novel way is known as the Internet of Things (IoT). Automation, environmental analysis, and health and safety awareness are large areas of study that can benefit from IoT networks, but issues with compatibility, versatility, and cost inhibit implementation of such networks. The purpose of this project is to create an inexpensive, robust, low-power IoT sensor network to serve as an easily implementable model for individuals, communities, and cities. Additionally, the network can serve as a tool within other areas of research for experimentation and QA analysis. The foundation of this system was successfully created using Arduino-based processing in tandem with packet radios that communicate over sub-GHz frequency bands. Ethernet data packet management was done using MQ Telemetry Transport (MQTT) and data visualization was performed with Grafana and MATLAB. This network also utilizes InfluxDB to store and manage data effectively.


## Contents
- [Quick start](#quickstart)
  - [Necessary hardware](#necessaryhardware)
  - [Necessary tools](#necessarytools)
  - [Hardware setup](#hardwaresetup)
    - [RFM69 Arduino](#rfm69arduino)
    - [Ethernet Arduino](#ethernetarduino)
  - [Network setup](#networksetup)
- [Research Analysis](#ra)
  - [Cost Scaling](#cost)
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
Below are the steps to initialize the server, gateway, and one sensor node.
1. **Ethernet and Radio Gateway node setup**

   Program Radio Gateway Arduino Uno with DragonHomeNetwork/Arduino/rfm69-network/gateway_uart_mqtt. This node recieves Radio transmissions from every sesnor node and forwards them to the Ethernet node vio Serial TX/RX. TODO Include images 288-89, 291-93.

   Program Ethernet-enabled Arduino Uno with DragonHomeNetwork/Arduino/rfm69-network/uart-mqtt script. This node receives Serial TX/RX information from the radio gateway and forwards information to the mqtt server via ethernet. In the uart_mqtt script, the user must enter the IP address of the mqtt server.

   Connect both Arduino nodes to a wall power adapter to turn them on.

   Connect pin 0 on the Ethernet Arduino to pin 1 on the radio gateway, and pin 0 on the radio gateway to pin 1 on the Ethernet Arduino. Connect the GND pin on the Ethernet arduino to the GND pin on the gateway arduino. This serves as the Serial TX/RX communication between the nodes.   

   Note: When uploading code or powering on either board, ensure there are no wires connected between them (Remove TX/RX connection!).

2. **Server setup.**

    Install mosquitto and mosquitto-clients, then enable mosquitto daemon:

    ```
    sudo apt-get install mosquitto mosquitto-clients
    sudo systemctl enable mosquitto
    ```

    TODO Other software installs
    Make sure, if you haven't already, to clone this repository. Also install the other dependent software below.
    ```
    git clone https://github.com/DrexelSmartHouse/DragonHomeNetwork.git
    sudo apt-get install python-pip
    sudo pip install paho-mqtt
    sudo apt-get install tmux

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

   Install simple dht library through the Arduino IDE.

   In the DragonHomeNetwork/Arduino/rfm69-network/dht11_sensor_node script, change the node_id (line 13) to match the number you wish to give the end node on the network. This variable helps you to identify where data is coming from when there are multiple sensor nodes on the network.

   Upload this script to the sensor end node equipped with the DHT11 Temperature and Humidity sensor.

## Research Analysis <a name="ra"></a>
### Cost Scaling <a name="cost"></a>
The following table documents the cost of the core of the network - the Ethernet and gateway nodes.
| Component                                                                   | Quantity | Price   |
|-----------------------------------------------------------------------------|----------|---------|
| [Arduino Uno]("http://amzn.to/2o2KFku")                                     | 2        | $15.98  |
|  [Proto Shield with Mini Breadboard]("http://amzn.to/2H5cfGe")              | 1        | $7.99   |
| [RFM69HCW Wireless Transciever - 915MHz (4 pack)]("http://amzn.to/2nYT9sM") | 1        | $21.95  |
| [10Kohm Resistor (400-pack)]("http://amzn.to/2BqvdXk")                      | 1        | $8.65   |
| [9V 1A Arduino Power Supply]("http://amzn.to/2EWiUSA")                      | 1        | $6.99   |
| [Voltage/logic converter]("http://amzn.to/2EWT3tM")                         | 1        | $5.39   |
| [Solid core wire (ft)]("http://amzn.to/2BSlPgd")                            | 2        | $39.99  |
| [Ethernet Shield]("http://bit.ly/2EPkiZq")                                  | 1        | $32.50  |
| [3ft Ethernet cable]("http://bit.ly/2C3T68h")                               | 1        | $2.50   |
| Total                                                                       |          | $141.94 |

The next table documents the cost of an additional node.
| Component                                                                   | Quantity | Cost   |
|-----------------------------------------------------------------------------|----------|--------|
| [Arduino Uno]("http://amzn.to/2o2KFku")                                     | 1        | $7.99  |
|  [Proto Shield with Mini Breadboard]("http://amzn.to/2H5cfGe")              | 1        | $7.99  |
| [RFM69HCW Wireless Transciever - 915MHz (4 pack)]("http://amzn.to/2nYT9sM") | 1        | $21.95 |
| [10Kohm Resistor (400-pack)]("http://amzn.to/2BqvdXk")                      | 1        | $8.65  |
| [9V 1A Arduino Power Supply]("http://amzn.to/2EWiUSA")                      | 1        | $6.99  |
| [Voltage/logic converter]("http://amzn.to/2EWT3tM")                         | 1        | $5.39  |
| [Solid core wire (ft)]("http://amzn.to/2BSlPgd")                            | 2        | $39.99 |
| Total                                                                       |          | $98.95 |

The final table documents how cost scales as _n_ nodes are added to the network.
| Nodes | Cost      |  Network total cost |
|-------|-----------|---------------------|
| 1     | $98.95    | $240.89             |
| 4     | $184.03   | $325.97             |
| 16    | $590.20   | $732.14             |
| 40    | $1402.54  | $1544.48            |
| 160   | $13627.63 | $13769.57           |


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
