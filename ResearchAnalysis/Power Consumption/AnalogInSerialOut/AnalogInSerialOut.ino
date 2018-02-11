/*
  Analog input, serial output

  Reads an analog input pin and prints the results to the Serial Monitor for power consumption analysis.

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe
  modified 11 Feb 2018

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot

void setup() {
  Serial.begin(115200);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);

  // print the results to the Serial Monitor:
  Serial.println(sensorValue);

  delay(1000);
}

