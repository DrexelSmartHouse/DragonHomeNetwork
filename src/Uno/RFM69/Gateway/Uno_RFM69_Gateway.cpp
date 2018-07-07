/**************************************************************
 * Arduino file to create a gateway for the DragonHome Network.
 * The gateway requests data from the nodes and once the data
 * is received it sends it off to the to the serial>ethernet
 * Arduino.
 **************************************************************/

#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF69 driver;
// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

const unsigned long time_out_ms = 1000;
unsigned long start_time = 0;

volatile bool receiving = false;
volatile bool sweep_mode = false;

uint8_t current_node_id = 1;

// function prototypes
void publishSensorReading();
void publishLogMsg(String msg);
void restartTimer();
bool timeOut();


/**************************************************************
 * Function: setup
 * ------------------------------------------------------------ 
 * summary: initializes serial interface and radio and updates
 * the list of live nodes
 * parameters: void
 * return: void
 **************************************************************/
void setup()
{
  Serial.begin(9600);

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  if (!manager.init())
    Serial.println("init failed");

  if (!driver.setFrequency(915.0))
    Serial.println("setFrequency failed");
  driver.setTxPower(14, true);

    // The encryption key has to be the same as the one in the client
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  driver.setEncryptionKey(key);
  
}
uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

/**************************************************************
 * Function: loop
 * ------------------------------------------------------------ 
 * summary: if else structure for error handling, requesting, 
 * and receiving data from nodes
 * parameters: void
 * return: void
 **************************************************************/
void loop()
{

  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char *)buf);  

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
}

/**************************************************************
 * Function: publishSensorReading
 * ------------------------------------------------------------ 
 * summary: sends sensor reading over serial 
 * structure: /{node id}/{sensor type}:{data}\n
 * example: /1/TEMPC:40.1
 * parameters: void
 * return: void
 **************************************************************/
void publishSensorReading()
{

  Serial.print('/');
  Serial.print(CLIENT_ADDRESS);
  Serial.print('/');
  //TODO: Fix sensor type and sensor data parsing through serial
  Serial.print("SENSOR TYPE HERE");
  Serial.print(':');
  Serial.print("SENSOR DATA HERE");
  Serial.print('\n');
}

/**************************************************************
 * Function: publishLogMsg
 * ------------------------------------------------------------ 
 * summary: sends log messages over serial
 * parameters: string msg
 * return: void
 **************************************************************/
void publishLogMsg(String msg)
{
  Serial.print(F("/log:"));
  Serial.print(msg);
  Serial.print('\n');
}

/**************************************************************
 * Function: restartTimer
 * ------------------------------------------------------------ 
 * summary: resets start_time 
 * parameters: void
 * return: void
 **************************************************************/
void restartTimer()
{
  start_time = millis();
}

/**************************************************************
 * Function: timeOut
 * ------------------------------------------------------------ 
 * summary: returns boolean if a send times out
 * parameters: void
 * return: bool 
 **************************************************************/
bool timeOut()
{
  return (millis() - start_time >= time_out_ms);
}

/**************************************************************
 * Function: incrementCurrNodeID
 * ------------------------------------------------------------ 
 * summary: returns boolean if next available node id is on 
 * available_nodes list
 * parameters: void
 * return: bool
 **************************************************************/
bool incrementCurrNodeID()
{
}

/**************************************************************
 * Function: requestAllFromNextNode
 * ------------------------------------------------------------ 
 * summary: finds the next node and requests data from it
 * if the next node_id isn't on the available_nodes list then 
 * it returns false
 * parameters: void
 * return: bool
 **************************************************************/
bool requestAllFromNextNode()
{
  // TODO: Make this again?
  // Probably not, since we will be using a new system not based on requests
}

/**************************************************************
 * Function: startNetworkSweep
 * ------------------------------------------------------------ 
 * summary: initializes a network sweep and calls 
 * requestAllFromNextNode, returns error message if node is busy
 * parameters: void
 * return: void
 **************************************************************/
void Sweep()
{
  // TODO: Make this again?
}

/**************************************************************
 * Function: networkScan
 * ------------------------------------------------------------ 
 * summary: pings all nodes on the network and records which 
 * nodes are live
 * parameters: void
 * return: bool 
 **************************************************************/
void Scan()
{
  // TODO: Make this again?
}
