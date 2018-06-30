
/**************************************************************
* Arduino file to create a node for the DragonHome Network.
* The node polls for a request from the gateway for sensor 
* data and then responds to request.
**************************************************************/
#include "RFM69_DSH.h"
#include <Wire.h>
#include "Adafruit_MCP9808.h"

RFM69_DSH dsh_radio;

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

//Change depending on the node number programmed
const uint8_t NODE_ID = 10;
const uint8_t NETWORK_ID = 0;

const long sensor_interval = 1500;

const uint8_t DHT11_pin = 4;
byte temp = -1;
byte humidity = -1;
int rssi = 0;

//var for time
long previous_time = 0;

enum request_types
{
  ALL,
  TEMPC,
  HUM,
  RSSIDAT,
  BAD_REQUEST,
  NONE
};

request_types current_request = NONE;

/**************************************************************
* Function: setup
* ------------------------------------------------------------ 
* summary: Initializes serial and dsh_radio
* parameters: void
* return: void
**************************************************************/
void setup()
{
  Serial.begin(115200);

  dsh_radio.initialize(RF69_915MHZ, NODE_ID, NETWORK_ID);
  dsh_radio.setHighPower();

  if (!tempsensor.begin())
  {
    Serial.println("Couldn't find MCP9808!");
  }

  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: ");
  Serial.print(c);
  Serial.print("*C\t");
  Serial.print(f);
  Serial.println("*F");

  float temp = 0;
}

/**************************************************************
* Function: loop
* ------------------------------------------------------------ 
* summary: Loop polls for a request. If a request is received,
* it is parsed and sent through a switch to handle the request.
* parameters: void
* return: void
**************************************************************/
void loop()
{

  if (dsh_radio.receiveDone())
  {

    Serial.println("Transmission Received");

    if (dsh_radio.requestReceived())
    {
      Serial.println("" + String(dsh_radio.readRSSI()));
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

    if (dsh_radio.ACKRequested())
    {
      dsh_radio.sendACK();
      Serial.println("ACK sent");
    }
  }

  switch (current_request)
  {
  case NONE:
    break;

  case BAD_REQUEST:
    dsh_radio.sendError("BAD REQUEST");
    current_request = NONE;
    break;

  case ALL:
    temp = tempsensor.readTempC();
    Serial.println("Sending all sensor Data");

    if (!dsh_radio.sendSensorReading("TEMPC", temp))
    {
      dsh_radio.sendError("TEMPC SEND");
      Serial.println("Temp Transmission Failed");
      current_request = NONE;
      break;
    }

    if (!dsh_radio.sendSensorReading("RSSIDAT", dsh_radio.readRSSI()))
    {
      dsh_radio.sendError("RSSI SEND");
      Serial.println("RSSI Transmission Failed");
      current_request = NONE;
      break;
    }

    dsh_radio.sendEnd();
    current_request = NONE;
    break;

  case TEMPC:
    temp = tempsensor.readTempC();
    if (!dsh_radio.sendSensorReading("TEMPC", temp))
      Serial.println("Temp Transmission Failed");
    dsh_radio.sendEnd();
    current_request = NONE;
    break;

  case RSSIDAT:
    if (!dsh_radio.sendSensorReading("RSSI", dsh_radio.readRSSI()))
      Serial.println("RSSI Transmission Failed");
    dsh_radio.sendEnd();
    current_request = NONE;
    break;

  default:
    current_request = NONE;
  }
}
