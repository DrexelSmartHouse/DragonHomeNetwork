/**************************************************************
 * Arduino file to create a gateway for the DragonHome Network.
 * The gateway requests data from the nodes and once the data
 * is received it sends it off to the to the serial>ethernet
 * Arduino.
 **************************************************************/

#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <SPI.h>

#if defined(__AVR_ATmega32U4__) // Feather 32u4 w/Radio
#define RFM69_CS 8
#define RFM69_INT 7
#define RFM69_RST 4
#define LED 13
#define radio() driver(RFM69_CS, RFM69_INT)
#else
#define RFM69_RST 0
#define LED 13
#define radio() driver
#endif

#define SERVER_ADDRESS 0

// Singleton instance of the radio driver. Managed by preprocessor directives
RH_RF69 radio();
// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

// function prototypes
void publishSensorReading();
void publishLogMsg(String msg);

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
  uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  driver.setEncryptionKey(key);
}

uint8_t data[] = "Acknowledged";
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t dataRec[sizeof(buf)];
char *vals;

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
      vals = strtok((char *)buf, ",:");
      while (vals != NULL)
      {
        Serial.print("DHN/");
        Serial.print(SERVER_ADDRESS, DEC);
        Serial.print('/');
        Serial.print(from, DEC);
        Serial.print('/');
        Serial.print(vals);
        Serial.print('/');
        vals = strtok(NULL, ",:");
        Serial.print(vals);
        vals = strtok(NULL, ",:");
        Serial.print('\n');
      }
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("Ack failed");
    }
  }
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