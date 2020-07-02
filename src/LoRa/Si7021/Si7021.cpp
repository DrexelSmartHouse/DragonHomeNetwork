#include <Wire.h>
#include "Adafruit_Si7021.h"
#include "../RFM95.h"

// Singleton instance of the radio driver
RFM95 radio;

// Create the Si7021 temperature and humid sensor object
Adafruit_Si7021 tempHumidSensor = Adafruit_Si7021();

// Function prototypes
int8_t celciusToFahrenheit(int8_t c);

char data[30];
char buffer[10];

void setup()
{
    radio.initRadio();

    if (!tempHumidSensor.begin())
        Serial.println("Couldn't find Si7021!");
}

void loop()
{
    strcpy(data, "TEMPF:");
    itoa(celciusToFahrenheit(tempHumidSensor.readTemperature()), buffer, 10);
    strcat(data, buffer);
    strcat(data, ":HUMID%RH:");
    itoa(tempHumidSensor.readHumidity(), buffer, 10);
    strcat(data, buffer);
    strcat(data, '\0');
    Serial.println(data);

    radio.sendMessage(data, 30);
    
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