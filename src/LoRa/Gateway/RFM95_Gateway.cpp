#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>

//Feather32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define radio() rf95(RFM95_CS, RFM95_INT)
#define RF95_FREQ 915.0
#define SERVER_ADDRESS 0

// Singleton instance of the radio driver. Managed by preprocessor directives
RH_RF95 radio();

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

// function prototypes
void publishSensorReading();
void publishLogMsg(String msg);

void setup()
{
    Serial.begin(9600);

    if (!manager.init())
        Serial.println("init failed");
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    if (!rf95.setFrequency(RF95_FREQ))
        Serial.println("setFrequency failed");

    rf95.setTxPower(23, false);
}

uint8_t data[] = "Acknowledged";
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t dataRec[sizeof(buf)];
char *vals;

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
                Serial.print("DHN/");
                Serial.print(SERVER_ADDRESS, DEC);
                Serial.print('/');
                Serial.print(from, DEC);
                Serial.print('/');
                Serial.print('acknowledgementfailed');
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