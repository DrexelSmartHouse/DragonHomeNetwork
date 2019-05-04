#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"

//Feather32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define radio() rf95(RFM95_CS, RFM95_INT)

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 radio();

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 0

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

// Function prototypes
int8_t celciusToFahrenheit(int8_t c);
void printTempF();
void blink(byte PIN, byte DELAY_MS, byte loops);

void setup()
{
    Serial.begin(9600);

    pinMode(LED, OUTPUT);
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);

    if (!manager.init())
        Serial.println("init failed");
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:

    if (!rf95.setFrequency(RF95_FREQ))
        Serial.println("setFrequency failed");

    if (!tempsensor.begin())
        Serial.println("Couldn't find MCP9808!");

    rf95.setTxPower(23, false);

    printTempF();
}

uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
char data[RH_RF95_MAX_MESSAGE_LEN];
char buffer[10];

void loop()
{
    // RSSI isn't accurate, cause it returns the value of the last message (the one before what we are sending)
    // TODO: Make the RSSI a gateway side datapoint
    strcpy(data, "RSSI:");
    itoa(rf95.lastRssi(), buffer, 10);
    strcat(data, buffer);
    strcat(data, ",TEMPF:");
    itoa(celciusToFahrenheit(tempsensor.readTempC()), buffer, 10);
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
            blink(LED, 40, 3);
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
  Serial.print(celciusToFahrenheit(tempsensor.readTempC()));
  Serial.print("*F\n");
}

/**************************************************************
* Function: blink
* ------------------------------------------------------------ 
* summary: Function fr configuring LED blinking
* parameters: 
*     byte PIN: The pin for the LED
*     byte DELAY_MS: Delay between LED blinks
*     byte loops: Number of led blinks
* return: void
**************************************************************/
void blink(byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i = 0; i < loops; i++)
  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}