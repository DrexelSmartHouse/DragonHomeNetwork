#include <SimpleDHT.h>
#include "../RFM69.h"

#define DHT11_PIN 4

// Singleton instance of the radio driver
RFM69 radio;

//Create DHT11 dht11 object
SimpleDHT11 dht11;

// Function prototypes
int8_t celciusToFahrenheit(int8_t c);

char data[20];
char buffer[10];
byte temp = -1;
byte humidity = -1;

void setup()
{
  radio.initRadio();
}

void loop()
{
  strcpy(data, "TEMPF:");
  dht11.read(DHT11_PIN, &temp, &humidity, NULL);
  itoa(celciusToFahrenheit(temp), buffer, 10);
  strcat(data, buffer);
  strcat(data, '\0');
  Serial.println(data);

  radio.sendMessage(data, 20);

  // Sends data every 5 seconds
  delay(50000);
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
