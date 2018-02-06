/**************************************************************
 * Arduino file to create a node for the DragonHome Network.
 * The node polls for a request from the gateway for sensor 
 * data and then responds to request.
 **************************************************************/

#include "RFM69_DSH.h"
#include <SimpleDHT.h>

RFM69_DSH dsh_radio;

const uint8_t node_id = 7;
const uint8_t network_id = 0;

const long sensor_interval = 1500;

const uint8_t DHT11_pin = 4;
byte temp = -1;
byte humidity = -1;

SimpleDHT11 dht11;

//var for time
long previous_time = 0;

enum request_types {
  ALL,
  TEMPC,
  HUM,
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

	dsh_radio.initialize(RF69_915MHZ, node_id, network_id);
	dsh_radio.setHighPower(false);

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
  
	if (dsh_radio.receiveDone()) {

		Serial.println("Transmission Received");

		
    if (dsh_radio.requestReceived()) {
      if (dsh_radio.requestAllReceived())
        current_request = ALL;
      else if (dsh_radio.getReceivedStr() == "HUM")
        current_request = HUM;
      else if (dsh_radio.getReceivedStr() == "TEMPC")
        current_request = TEMPC;
      else
        current_request = BAD_REQUEST;
    }
    
		if (dsh_radio.ACKRequested()) {
			dsh_radio.sendACK();
			Serial.println("ACK sent");
		}
   
	}
 
  switch(current_request) {
    case NONE:
      break;

    case BAD_REQUEST:
      dsh_radio.sendError("BAD REQUEST");
      current_request = NONE;
      break;
    
    case ALL:
      dht11.read(DHT11_pin, &temp, &humidity, NULL);
      Serial.println("Sending all sensor Data");

      if (!dsh_radio.sendSensorReading("TEMPC", temp)) {
        dsh_radio.sendError("TEMPC SEND");
        Serial.println("Temp Transmission Failed");
        current_request = NONE;
        break;
      }

      if (!dsh_radio.sendSensorReading("HUM", humidity)) {
        dsh_radio.sendError("HUM SEND");
        Serial.println("Hum Transmission Failed");
        current_request = NONE;
        break;
      }
      
      dsh_radio.sendEnd();
      current_request = NONE;
      break;
       
    case TEMPC:
      dht11.read(DHT11_pin, &temp, &humidity, NULL);
      if (!dsh_radio.sendSensorReading("TEMPC", temp))
        Serial.println("Temp Transmission Failed");
      dsh_radio.sendEnd();
      current_request = NONE;
      break;
      
    case HUM:
      dht11.read(DHT11_pin, &temp, &humidity, NULL);
      if (!dsh_radio.sendSensorReading("HUM", humidity))
        Serial.println("Hum Transmission Failed");
      dsh_radio.sendEnd();
      current_request = NONE;
      break;

    
    default:
      current_request = NONE;
 
  }

  
  
}

