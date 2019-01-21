
/**************************************************************
* Arduino file to create a node for the DragonHome Network.
* The node sends data to the gateway at a given time interval.
**************************************************************/
#include <SPI.h>
#include <RH_RF69.h>
#include <SimpleDHT.h>
#include <RHReliableDatagram.h>

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

#define CLIENT_ADDRESS 12
#define SERVER_ADDRESS 0

// Singleton instance of the radio driver
RH_RF69 radio();

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

//Create DHT11 dht11 object
SimpleDHT11 dht11;

const uint8_t DHT11_pin = 4;
byte temp = -1;
byte humidity = -1;

// Function prototypes
int8_t celciusToFahrenheit(int8_t c);
void printTempF();
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

  printTempF();
}

uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
char data[RH_RF69_MAX_MESSAGE_LEN];
char buffer[10];

/**************************************************************
* Function: loop
* ------------------------------------------------------------ 
* summary: Loop sends a set of data at a specific time interval
* parameters: void
* return: void
**************************************************************/
void loop()
{
  strcpy(data, "RSSI:");
  itoa(driver.rssiRead(), buffer, 10);
  strcat(data, buffer);
  strcat(data, ",TEMPF:");
  dht11.read(DHT11_pin, &temp, &humidity, NULL);
  itoa(celciusToFahrenheit(temp), buffer, 10);
  strcat(data, buffer);
  strcat(data, '\0');
  Serial.println(data);
  // Send a message to manager_server
  if (manager.sendtoWait((uint8_t *)data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("Recieved reply from node ");
      Serial.print(from, DEC);
      Serial.print(": \n");
      Serial.println((char *)buf);
    }
    else
    {
      Serial.println("No reply, is gateway running?\n");
    }
  }
  else
    Serial.println("Sending failed.\n");
  // Sends data every ten seconds
  delay(10000);
}

/**************************************************************
* Function: celciusToFahrenheit
* ------------------------------------------------------------ 
* summary: Converts a given temperature in Celcius to its Fahrenheit 
* parameters: int8_t Temperature in Celcius
* return: int8_t Temperature in Fahrenheit
**************************************************************/
int8_t celciusToFahrenheit(int8_t c)
{
  return (c * 9.0) / 5.0 + 32;
}

/**************************************************************
* Function: printTempF
* ------------------------------------------------------------ 
* summary: Prints the temperature from the MCP9808 sensor to serial
* parameters: void
* return: void
**************************************************************/
void printTempF()
{
  Serial.print("Temp: ");
  dht11.read(DHT11_pin, &temp, &humidity, NULL);
  Serial.print(celciusToFahrenheit(temp));
  Serial.print("*F\n");
}
