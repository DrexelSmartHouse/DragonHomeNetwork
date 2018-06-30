
#define __AVR_ATmega32U4__
#include "RFM69_DSH.h"
#include <SimpleDHT.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

RFM69_DSH dsh_radio(8, 7, true, digitalPinToInterrupt(7));

const uint8_t node_id = 15;
const uint8_t network_id = 0;

//var for time
long previous_time = 0;

enum request_types
{
  ALL,
  TEMPC,
  BAD_REQUEST,
  NONE
};

request_types current_request = NONE;

void setup()
{

  Serial.begin(115200);

  dsh_radio.initialize(RF69_915MHZ, node_id, network_id);
  //dsh_radio.setCS(8);
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
}

float temp = 0;

void loop()
{

  if (dsh_radio.receiveDone())
  {

    Serial.println("Transmission Received");

    if (dsh_radio.requestReceived())
    {
      if (dsh_radio.requestAllReceived())
        current_request = ALL;
      else if (dsh_radio.getReceivedStr() == "TEMPC")
        current_request = TEMPC;
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

  default:
    current_request = NONE;
    break;
  }
}
