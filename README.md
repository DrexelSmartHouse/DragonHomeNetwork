# Updated as of 10/26/20. Will be updated in the near future.

# DragonHomeNetwork - Building Autonomy and Simulation Lab Branch
Our wireless network is based on the Arduino platform and include many sensors and form factors. The original development is credited to Joshua Cohen’s team (including Reno Farnesi, Ryan Hassing and Timothy Lechman) and a previous Drexel Smart House research project: The Dragon Home Network.

The project's final goal is to build a robust and low-power IoT sensor network to serve a indoor environmetal data collection system, which can integrated into an autonomous flying drone and on the ground rover. With the entire system in place, the full monitoring system can be used to investigate building-related problems and solutions in energy conservation, occupant cofort and health. The core of the system is based on network of microcontroller boards Feather 32u4 with built-in LoRa packet radio transceiver, which initiates communication over 915 MHz frequency bands. Ethernet data packet management was done using MQ Telemetry Transport (MQTT), and local data storage is implemented. This network also utilizes the same InfluxDB, which is Time Series Database to store and manage data effectively.

More information on our **"Building Autonomy and Simulation Lab"** can be found [here](https://research.coe.drexel.edu/caee/basl/)

![Our Works](/images/ourWorks.png)

## Contents
- [Features](#features)
- [Quick start](#quickstart)
  - [Necessary hardware](#necessaryhardware)
  - [Necessary tools](#necessarytools)
  - [Hardware setup](#hardwaresetup)
  - [Software setup](#softwaresetup)
  - [Network setup](#networksetup)
- [Research Analysis](#ra)
  - [Cost Scaling](#cost)
- [Releases](#releases)
- [Contributors](#contributors)

## Features <a name="features"></a>
The completed network has the following features:
- Modular addition and modification of sensors,
- Low cost ($34.95 for the gateway node and $34.95 for each additional node on the network, with efficient cost scaling)
https://www.adafruit.com/product/3078
- Power consumption: ~40mA during active radio listening, ~120mA transmitting.
- Packet transmission up to 200 meters.

![Final Build of Sensors and Gateway](/images/builtSensors.png)

## Quick start <a name="quickstart"></a>
### Necessary hardware <a name="necessaryhardware"></a>
The following hardware is necessary to set up the network and connect one node:
- (2) Arduino Proto Shield with Mini Breadboard
- (2) Adafruit Feather 32u4 RFM95 LoRa Radio - 915MHz
- (1) MCP9808 High Accuracy I2C Temperature sensor
- (1) 9V 1A Arduino power supplies
- Solid core wires
- Solder

### Necessary tools <a name="necessarytools"></a>
- Soldering iron
- Wire strippers
- Arduino/USB cable
- Internet-connected computer

### Hardware setup <a name="necessarytools"></a>

Follow the below schematic to set up your nodes:
todo include image.

### Software setup <a name="softwaresetup"></a>
Visual Studio Code, for its built-in Git commands and easy installation of required extensions. PlatformIO IDE is installed as an extension in VSCode due to is optimization for embedded C/C++ development for the gateway and sensors:
- [Visual Studion Code](https://code.visualstudio.com/) should be downloaded and installed first.
- Then, [PlatformIO](https://maker.pro/arduino/tutorial/how-to-use-platformio-in-visual-studio-code-to-program-arduino) will be installed insided VS Code as an extension.
### Network setup <a name="networksetup"></a>

Below are the steps to initialize the server, gateway and one sensor node.

1. **Gateway node setup**

    Program Radio Gateway with `DragonHomeNetwork/src/LoRa/Gateway/Gateway.cpp`. This node receives Radio transmissions from every sensor node and forwards them to the mqtt server via USB.

    In `DragonHomeNetwork/platformio.ini`, change to ```default_envs = F32u4_RFM95_Gateway``` in order to upload the Gateway script to the gateway node.

    Connect the Arduino node to your server.


2. **Server setup.**

    Open `bash` on your server to execute the following commands.

    Clone the repository:
    ```
    git clone https://github.com/DrexelSmartHouse/DragonHomeNetwork.git
    ```
    Go to Professor James Lo branch:
    ```
    git checkout lo_lab
    ```
    Install mosquitto and mosquitto-clients, then enable mosquitto daemon:

    ```
    sudo apt-get install mosquitto mosquitto-clients
    sudo systemctl enable mosquitto
    ```

    Install the other dependent software below.
    ```
    sudo apt-get install python-pip
    sudo pip install paho-mqtt
    sudo apt-get install tmux

    ```

    Make sure that the mqtt server IP is the same as the IP address in `serial-to-mqtt.conf`, which can be accessed by the below instructions:
    ```
    cd /path/to/DataCollectionAndAnalysis
    vim serial-to-mqtt.conf
    ```

    Open a tmux session in the cloned `DataCollectionAndAnalysis` directory and run the following scripts.

    ```
    tmux new -s dsh
    ./rfm69_mqtt_manager.py
    CTRL+B SHIFT+5 to open a parallel window.
    ./rfm69-serial-to-mqtt.py
    ```

     Now you can view sweep and scan events, as well as the sensor data and log messages as they are generated. To leave the server running, type `CTRL+B d`. To bring the session back, type `tmux a`. To switch between parallel windows, type `CTRL+B` and use the arrow keys.


3. **Sensor node setup**

   With PlatformIO installed with VS code, MCP9808 library should be already installed.

   In the `DragonHomeNetwork/src/LoRa/RFM95.cpp`, change the SERVER_ADDRESS (line 7) to match the number you wish to give the end node on the network. This variable helps you to identify where data is coming from when there are multiple sensor nodes on the network.

   In `DragonHomeNetwork/platformio.ini`, change to ```default_envs = F32u4_RFM69_MCP9808``` in order to upload the `DragonHomeNetwork/src/LoRa/MCP9808/MCP9808.cpp` to the sensor end node equipped with the MCP9808 Temperature sensor.

   This node can be powered via USB or wall power. Once powered on, it will automatically send data to the server.

## Research Analysis <a name="ra"></a>
### Cost Scaling <a name="cost"></a>
The following table documents the cost of the core of the network - the Ethernet and gateway nodes.

| Component                                                                   | Price   |
|-----------------------------------------------------------------------------|---------|
|  [Proto Shield with Mini Breadboard](https://www.aliexpress.com/item/Newbrand-Prototyping-Prototype-Shield-ProtoShield-Mini-Breadboard-for-Arduino-Free-Shipping/32279103685.html?spm=2114.search0104.3.112.2fb358f9cm0yb2&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711212_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5722413_5711312,searchweb201603_2,ppcSwitch_5&algo_expid=c98ab005-6807-4240-8d2e-648ed088282c-19&algo_pvid=c98ab005-6807-4240-8d2e-648ed088282c&transAbTest=ae803_5&priceBeautifyAB=0)              | $1.10   |
| [Feather 32u4 RFM95 - 915MHz (2 pack)](https://www.adafruit.com/product/3078) | $69.90 |
| [MCP9808 Temperature sensor](https://www.adafruit.com/product/1782?gclid=Cj0KCQjw59n8BRD2ARIsAAmgPmKS2YybZiqMzAKGVtOok_7Qck5UNR63AhICCtJkE-IcmJ4vnps6N6IaAuyBEALw_wcB)                                     | $4.95  |
| [9V 1A Arduino Power Supply](https://www.aliexpress.com/item/US-Plug-9V-1A-Switching-Power-Supply-Converter-Adapter-Wall-Charger-4-0mmx1-7mm/32849270931.html?spm=2114.search0104.3.109.c56333bd76YIdY&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711213_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711313_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=22c8c7c1-85ab-437a-bb54-977e7bd0358f-16&algo_pvid=22c8c7c1-85ab-437a-bb54-977e7bd0358f&transAbTest=ae803_5&priceBeautifyAB=0)                      | $2.08   |
| [Solid core wire](https://www.aliexpress.com/item/Tinned-copper-22AWG-2-pin-Red-Black-cable-PVC-insulated-wire-22-awg-wire-Electric-cable/32597557917.html?spm=2114.search0104.3.2.2f6215c6LjDKMM&ws_ab_test=searchweb0_0,searchweb201602_3_10152_10151_10065_10344_10068_10130_10324_10342_10547_10325_10343_10546_10340_10548_10341_10545_10084_10083_10618_10307_5711213_10313_10059_10534_100031_10103_10627_10626_10624_10623_10622_10621_10620_5711313_5722413,searchweb201603_2,ppcSwitch_5&algo_expid=34901f3e-f79f-4489-9022-3fd66b59f083-0&algo_pvid=34901f3e-f79f-4489-9022-3fd66b59f083&transAbTest=ae803_5&priceBeautifyAB=0)                            | $0.48  |
| Total (+ shipping)                                               | $78.51 |



The final table documents how cost scales as _n_ nodes are added to the network.

| Nodes | Cost      |
|-------|-----------|
| 1     | $34.95    |
| 10    | $349.5   |


## Releases <a name="releases"></a>
Not currently released.

## Contributors <a name="contributors"></a>
The DragonHome Network team:
- [Joshua Cohen](https://github.com/jcohen98)
- [Reno Farnesi](https://github.com/nfarnesi4)
- [Ryan Hassing](https://github.com/ryanhassing)
- [Timothy Lechman](https://github.com/tlechman49)

Building Autonomy and Simulation Lab - Sensor team:
- [Long Trinh](https://github.com/trnhx001)