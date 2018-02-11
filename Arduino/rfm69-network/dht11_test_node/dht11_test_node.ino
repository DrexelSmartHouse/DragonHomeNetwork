
/**************************************************************
  Arduino file to create a node for the DragonHome Network.
  The node polls for a request from the gateway for sensor
  data and then responds to request.
**************************************************************/
#include "RFM69_DSH.h"
#include <SimpleDHT.h>

RFM69_DSH dsh_radio;

//Change depending on the node number programmed
const uint8_t node_id = 3;
const uint8_t network_id = 0;

const long sensor_interval = 1500;

const uint8_t DHT11_pin = 4;
byte temp = -1;
byte humidity = -1;
int rssi = 0;
String reader = "";
boolean stand;

SimpleDHT11 dht11;

//var for time
long previous_time = 0;

enum request_types {
  ALL,
  TEMPC,
  HUM,
  RSSIDAT,
  BAD_REQUEST,
  NONE
};

request_types current_request = NONE;

/**************************************************************
  Function: setup
  ------------------------------------------------------------
  summary: Initializes serial and dsh_radio
  parameters: void
  return: void
**************************************************************/
void setup()
{
  Serial.begin(115200);
  dsh_radio.initialize(RF69_915MHZ, node_id, network_id);
  dsh_radio.setHighPower();
  Serial.println("Radio On");
  Serial.println("Select your mode:");
}

/**************************************************************
  Function: loop
  ------------------------------------------------------------
  summary: Loop polls for a request. If a request is received,
  it is parsed and sent through a switch to handle the request.
  parameters: void
  return: void
**************************************************************/
void loop()
{

  //Give reader 1 of 3 modes
  // STANDBY: Only radio on. No sensor reading or sending/recieving.
  // TXRX: Radio on sending/recieving fake data. No sensor readings.
  // SENSOR: Sensor readings only in interval specified by sensor_interval
  reader = read_from_console(reader); //read unformatted input
  if (reader.length() > 0) { //if theres input in the serial monitor
    reader = format_console_input(reader); //format input
    delay(1000);

    if (reader.equals("STANDBY")) {
      reader = "";
      stand = true;
      Serial.println("Standby Mode. Type STOP to reset");
      while (stand) {
        reader = read_from_console(reader); //read unformatted input
        if (reader.length() > 0) { //if theres input in the serial monitor
          reader = format_console_input(reader); //format input
          delay(1000);
        }
        if (reader.equals("STOP")) {
          stand = false;
          reader = "";
        }
      }
      Serial.println("Select your mode:");
    }
    else if (reader.equals("TXRX")) {
      reader = "";
      stand = true;
      Serial.println("TXRX Mode. Type STOP to reset");
      while (stand) {
        reader = read_from_console(reader); //read unformatted input
        if (reader.length() > 0) { //if theres input in the serial monitor
          reader = format_console_input(reader); //format input
          delay(1000);
        }
        if (reader.equals("STOP")) {
          stand = false;
          reader = "";
        }
        if (dsh_radio.receiveDone()) {
          Serial.println("Transmission Received");

          if (dsh_radio.requestReceived()) {
            if (dsh_radio.requestAllReceived())
              current_request = ALL;
            else if (dsh_radio.getReceivedStr() == "HUM")
              current_request = HUM;
            else if (dsh_radio.getReceivedStr() == "TEMPC")
              current_request = TEMPC;
            else if (dsh_radio.getReceivedStr() == "RSSIDAT")
              current_request = RSSIDAT;
            else
              current_request = BAD_REQUEST;
          }

          if (dsh_radio.ACKRequested()) {
            dsh_radio.sendACK();
            Serial.println("ACK sent");
          }

        }

        switch (current_request) {
          case NONE:
            break;

          case BAD_REQUEST:
            dsh_radio.sendError("BAD REQUEST");
            current_request = NONE;
            break;

          case ALL:
            //dht11.read(DHT11_pin, &temp, &humidity, NULL);
            Serial.println("Sending fake Sensor Data");

            if (!dsh_radio.sendSensorReading("TEMPC", 255)) {
              dsh_radio.sendError("TEMPC SEND");
              Serial.println("Temp Transmission Failed");
              current_request = NONE;
              break;
            }

            if (!dsh_radio.sendSensorReading("HUM", 254)) {
              dsh_radio.sendError("HUM SEND");
              Serial.println("Hum Transmission Failed");
              current_request = NONE;
              break;
            }

            if (!dsh_radio.sendSensorReading("RSSIDAT", 200)) {
              dsh_radio.sendError("RSSI SEND");
              Serial.println("RSSI Transmission Failed");
              current_request = NONE;
              break;
            }

            dsh_radio.sendEnd();
            current_request = NONE;
            break;

          default:
            current_request = NONE;
        }
      }
      Serial.println("Select your mode:");
    }
    else if (reader.equals("SENSOR")) {
      reader = "";
      stand = true;
      Serial.println("Standby Mode. Type STOP to reset");
      while (stand) {
        reader = read_from_console(reader); //read unformatted input
        if (reader.length() > 0) { //if theres input in the serial monitor
          reader = format_console_input(reader); //format input
          delay(1000);
        }
        if (reader.equals("STOP")) {
          stand = false;
          reader = "";
        }
        dht11.read(DHT11_pin, &temp, &humidity, NULL);
        Serial.println("Reading DHT11");
        delay(sensor_interval);
      }
      Serial.println("Select your mode:");
    }
    reader = "";
  }
}

/**************************************************************
  Function: read_from_console
  ------------------------------------------------------------
  summary: Reads the serial input typed into the console and 
  returns the raw input
  parameters: String
  return: String
**************************************************************/
String read_from_console(String readVar) {
  if (Serial.available()) {
    readVar = Serial.readString();
  }
  return readVar;
}

/**************************************************************
  Function: format_console_input
  ------------------------------------------------------------
  summary: Parses raw serial input and returns parsed serial message
  parameters: String
  return: String
**************************************************************/
String format_console_input(String input) {
  input.replace("\r", "");
  input.replace("\n", "");
  Serial.println("Read: " + input);
  return input;
}
