#include "RFM69_DSH.h"

RFM69_DSH dsh_radio;

const uint8_t network_id = 0;

const uint8_t max_node_id = 20; // value between 1-255

const unsigned long time_out_ms = 1000;
unsigned long start_time = 0;

volatile bool receiving = false;
volatile bool sweep_mode = false;

bool available_nodes[max_node_id+1];

uint8_t current_node_id = 1;

// function protypes
void publishSensorReading();
void publishLogMsg(String msg);
void restartTimer();
bool timeOut();
bool incrementCurrNodeID();
bool requestAllFromNextNode();
void startNetworkSweep();
void networkScan();

void setup()
{
	Serial.begin(115200);

	dsh_radio.initialize(RF69_915MHZ, GATEWAY_ID, network_id);
	dsh_radio.setHighPower();

  //update the list of nodes that are alive
  networkScan();

}

void loop()
{

	if (dsh_radio.receiveDone()) {

    // restart the timer if were still receiving a stream of readings
    // and if the sender id matches the one that was requested
    //if (receiving && dsh_radio.SENDERID == current_node_id) {
       //restartTimer();
    //}
    
    if (dsh_radio.SENDERID != current_node_id && !dsh_radio.eventReceived())
       publishLogMsg("ERROR: OFF TIME " + String(dsh_radio.SENDERID));
    
    else if (dsh_radio.errorReceived()) {
      // format is ERROR: {node id} {error msg}
      publishLogMsg("ERROR: "
                    + String(dsh_radio.SENDERID)
                    + " "
                    + dsh_radio.getReceivedStr());
    }

    // send the string over serial to be published
    else if (dsh_radio.stringReceived()) {
      Serial.print(dsh_radio.getReceivedStr());
      Serial.print('\n');
    }

		else if (dsh_radio.endReceived()) {
			//Serial.println("End Received");
      restartTimer();
			receiving = false;
		}

		else if (dsh_radio.sensorReadingReceived()) {
      publishSensorReading();
		}

		// handle ACK
		if (dsh_radio.ACKRequested()) {
			dsh_radio.sendACK();
			//Serial.println("ACK sent");
		}

	}

   // request data from the next node
  // when we stop receiving
  if(!receiving && sweep_mode) {
    //delay(100);
    // if there are no nodes left to check end sweep mode
    if (!requestAllFromNextNode()) {
      sweep_mode = false;
    }
  }


  // something failed sending
  // send the error
  if(receiving && timeOut()) {
    publishLogMsg("ERROR: TIMEOUT NODE: " + String(current_node_id));
    receiving = false;
  }
 
}

void serialEvent()
{
  
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  // check for NETID command
  if (cmd == "NETID") {
    Serial.print(network_id);
    Serial.print('\n');
    return;
  }

  // echo the command for the log
  publishLogMsg("COMMAND: " + cmd);

  /* Parse the command */

  // check for a multpart cmd
  int8_t delim_pos = cmd.indexOf(':');
  if (delim_pos != -1) {
    
    // follows one of two patterns
    // request {node id}:{str request}
    // or event {node id}:EVENT:{str event}
  
    // parse the node id from beginning
    uint8_t node_id = cmd.toInt();
    
    if (node_id != 0) {
      // chop off the number and collon from the front
      cmd = cmd.substring(delim_pos+1);
      
      delim_pos = cmd.indexOf(':');

      // check for event
      if (delim_pos != -1) {

        // it is an event so send it
        if (cmd.substring(0, delim_pos) == "EVENT") {
          //dsh_radio.sendEvent();          
          return;
        }
        
      }

      // send the

      
      return;  
    }
    
  }
  
  // check for SWEEP command
  if (cmd == "SWEEP") {
    startNetworkSweep();
    return;
  }

  // update the list of nodes that are up
  if (cmd == "SCAN") {
    networkScan();
    return;
  }

  // clear all vars and update the list of nodes
  if (cmd == "RESET") {
    receiving = false;
    sweep_mode = false;
    current_node_id = 0;
    networkScan();
    return;
  }

  if (cmd == "STATUS") {
    if (sweep_mode) publishLogMsg("STATUS: SWEEPING");
    else if (receiving) publishLogMsg("STATUS: RECEIVING");
    else publishLogMsg("STATUS: OK");
    return;
  }

  // TODO: parse commands for specfic nodes
  // they would follow the format {node id}:{request} 
  // NOTE: request == "" and request == "ALL" both mean request all data
  
   publishLogMsg("ERROR: BAD COMMAND");
}

void publishSensorReading()
{
  // it needs to follow this pattern: /{node id}/{sensor type}:{data}\n
  // example: /1/TEMPC:40.1
  
  Serial.print('/');
  Serial.print(dsh_radio.SENDERID);
  Serial.print('/');
  Serial.print(dsh_radio.getSensorType());
  Serial.print(':');
  Serial.print(dsh_radio.getSensorData());
  Serial.print('\n');
  
}

void publishLogMsg(String msg)
{
  Serial.print(F("/log:"));
  Serial.print(msg);
  Serial.print('\n');
}

void restartTimer() {
  start_time = millis();
}

bool timeOut() {
  return (millis()-start_time >= time_out_ms);
}

// use the list of available node to find the next availble id
bool incrementCurrNodeID() {
  
  while(current_node_id < max_node_id) {
    ++current_node_id;
    
    if (available_nodes[current_node_id])
      return true;
  }
  // no more nodes alive nodes left
  return false;
}

 // find the next node and request data from it
 // if we are at the end then return false
bool requestAllFromNextNode() {
 
  if (!incrementCurrNodeID()) return false;
  
  //request the all and restart the timer
  if (dsh_radio.requestAll(current_node_id)) {
    receiving = true;
    restartTimer();
    return true;
  }
  else {
    receiving = false;
    publishLogMsg("ERROR: REQUEST FAILED: " + String(current_node_id));
    return false;
  } 
}

void startNetworkSweep()
{
  if (!receiving && !sweep_mode) {

    // set the vars to the initial states
    sweep_mode = true;
    current_node_id = 0;
    
    requestAllFromNextNode();
  } else {
    publishLogMsg("ERROR: BUSY");
  }
}

// ping all nodes on the network and update which are alive
void networkScan()
{
  for (uint8_t i = 1; i <= max_node_id; ++i) {
    available_nodes[i] = dsh_radio.ping(i);
    if (available_nodes[i])
      publishLogMsg("Found Node " + String(i));
  }
}

