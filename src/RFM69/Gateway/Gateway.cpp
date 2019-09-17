#include "../RFM69.h"

// Singleton instance of the radio driver. Managed by preprocessor directives
RFM69 radio;

// function prototypes
void publishSensorReading();

void setup()
{
  radio.initRadio();
}

void loop()
{
  radio.waitForMessage();
}