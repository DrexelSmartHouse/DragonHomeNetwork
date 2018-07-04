
/**************************************************************
* Arduino file to create a node for the DragonHome Network.
* The node polls for a request from the gateway for sensor 
* data and then responds to request.
**************************************************************/
#include <SPI.h>
#include <RH_RF69.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <RHReliableDatagram.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF69 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

//Change depending on the node number programmed
const uint8_t NODE_ID = 1;
const uint8_t NETWORK_ID = 0;

String data = "";

/**************************************************************
* Function: setup
* ------------------------------------------------------------ 
* summary: Initializes serial and RFM69 radio, as well as the radio manager
* parameters: void
* return: void
**************************************************************/
void setup()
{
  Serial.begin(9600);

  if (!manager.init())
    Serial.println("init failed");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  if (!driver.setFrequency(915.0))
    Serial.println("setFrequency failed");

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  driver.setTxPower(14, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  driver.setEncryptionKey(key);

  if (!tempsensor.begin())
  {
    Serial.println("Couldn't find MCP9808!");
  }

  // Read and print out the temperature, then convert to *F
  float f = tempsensor.readTempF();
  Serial.print("Temp: ");
  Serial.print(f);
  Serial.print("*F");
  float temp = 0;
}

// Dont put this on the stack:
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

/**************************************************************
* Function: loop
* ------------------------------------------------------------ 
* summary: Loop sends a set of data at a specific time interval
* parameters: void
* return: void
**************************************************************/
void loop()
{
  data = String("RSSI:") + driver.rssiRead() + "," + "TEMPF:" + tempsensor.readTempF();

  // Send a message to manager_server
  if (manager.sendtoWait(reinterpret_cast<uint8_t *>(&data[0]), sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char *)buf);
    }
    else
    {
      Serial.println("No reply, is rf69_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  // Sends data every minute
  delay(60000);
}
