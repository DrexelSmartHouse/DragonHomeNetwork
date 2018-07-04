
/**************************************************************
* Arduino file to create a node for the DragonHome Network.
* The node polls for a request from the gateway for sensor 
* data and then responds to request.
**************************************************************/
#include "RFM69.h"
#include <Wire.h>
#include "Adafruit_MCP9808.h"

#define IS_RFM69HW_HCW
//*********************************************************************************************
//Auto Transmission Control - dials down transmit power to save battery
//Usually you do not need to always transmit at max output power
//By reducing TX power even a little you save a significant amount of battery power
//This setting enables this gateway to work with remote nodes that have ATC enabled to
//dial their power down to only the required level (ATC_RSSI)
//#define ENABLE_ATC //comment out this line to disable AUTO TRANSMISSION CONTROL
//#define ATC_RSSI -80
RFM69 dsh_radio;

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

//Change depending on the node number programmed
const uint8_t NODE_ID = 1;
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

String data = "";
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
  dsh_radio.setPowerLevel(31);

  if (!tempsensor.begin())
  {
    Serial.println("Couldn't find MCP9808!");
  }
  else
  {
    Serial.println("Found MCP9808!");
  }
  Serial.println(dsh_radio.getFrequency());

  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: ");
  Serial.print(c);
  Serial.print("*C\t");
  Serial.print(f);
  Serial.println("*F");

  float temp = 0;

  Serial.println(dsh_radio.canSend());
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

    if (dsh_radio.receiveDone())
    {
      Serial.println("" + String(dsh_radio.readRSSI()));
      for (byte i = 0; i < dsh_radio.DATALEN; i++)
      {
        data += (char)dsh_radio.DATA;
      }
      if (data.equals("ALL"))
        current_request = ALL;
      else if (data.equals("HUM"))
        current_request = HUM;
      else if (data.equals("TEMPC"))
        current_request = TEMPC;
      else if (data.equals("RSSIDAT"))
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
    dsh_radio.sendWithRetry(1, "BAD_REQUEST", 12);
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
    if (!dsh_radio.sendSensorReading("TEMPC", temp) || temp >= 50)
      Serial.println("Temp Transmission Failed" + temp);
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
