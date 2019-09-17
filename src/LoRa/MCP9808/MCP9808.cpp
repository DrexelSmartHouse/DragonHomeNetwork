#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include "../RFM95.h"

// Singleton instance of the radio driver
RFM95 radio;

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

// Function prototypes
int8_t celciusToFahrenheit(int8_t c);

char data[20];
char buffer[10];

void setup()
{
    radio.initRadio();

    if (!tempsensor.begin())
        Serial.println("Couldn't find MCP9808!");
}

void loop()
{
    strcpy(data, "TEMPF:");
    itoa(celciusToFahrenheit(tempsensor.readTempC()), buffer, 10);
    strcat(data, buffer);
    strcat(data, '\0');
    Serial.println(data);

    radio.sendMessage(data, 20);

    // Sends data every 5 seconds
    delay(5000);
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