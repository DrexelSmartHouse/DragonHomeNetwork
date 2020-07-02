#include "RFM95.h"

// Singleton instance of the radio driver. Managed by preprocessor directives
RH_RF95 radio();

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

RFM95::RFM95()
{
}

void RFM95::initRadio()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    if (!manager.init())
        Serial.println("init failed");

    if (!rf95.setFrequency(RF95_FREQ))
        Serial.println("setFrequency failed");

    rf95.setTxPower(23, false);
}

void RFM95::waitForMessage()
{
    uint8_t data[] = "Acknowledged";
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    char *vals;

    if (manager.available())
    {
        // Wait for a message addressed to us from the client
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (manager.recvfromAck(buf, &len, &from))
        {
            vals = strtok((char *)buf, ",:");
            char temp[10];
            itoa(rf95.lastRssi(),temp,10);
            // For every sensor type sent.
            Serial.print("DHN/");
            Serial.print(SERVER_ADDRESS, DEC);
            Serial.print('/');
            Serial.print(from, DEC);
            while (vals != NULL && vals != temp)
            {
                Serial.print('/');
                Serial.print(vals);
                Serial.print('/');
                vals = strtok(NULL, ",:");
                Serial.print(vals);
                vals = strtok(NULL, ",:");      
            }
            Serial.print('\n');
            // Serial.print("DHN/");
            // Serial.print(SERVER_ADDRESS, DEC);
            // Serial.print('/');
            // Serial.print(from, DEC);
            // Serial.print('/');
            // Serial.print("RSSI/");
            // Serial.print(rf95.lastRssi(), DEC);
            // Serial.print('\n');
            // Send a reply back to the originator client
            if (!manager.sendtoWait(data, sizeof(data), from))
                Serial.println("Ack failed");
        }
    }
}

// TODO: Make function return boolean on pass/fail
void RFM95::sendMessage(char *message, uint8_t length)
{
    if (sizeof(message) > RH_RF95_MAX_MESSAGE_LEN)
    {
        Serial.println("Sending failed. Message too large for RFM95\n");
    }

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

    // Send a message to manager_server
    if (manager.sendtoWait((uint8_t *)message, length, SERVER_ADDRESS))
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
            blink(LED, 50, 3);
        }
        else
        {
            Serial.println("No reply, is gateway running?\n");
        }
    }
    else
        Serial.println("Sending failed.\n");
}

void RFM95::publishLogMsg(String msg)
{
    Serial.print(F("/log:"));
    Serial.print(msg);
    Serial.print('\n');
}

void RFM95::blink(byte PIN, byte DELAY_MS, byte loops)
{
    for (byte i = 0; i < loops; i++)
    {
        digitalWrite(PIN, HIGH);
        delay(DELAY_MS);
        digitalWrite(PIN, LOW);
        delay(DELAY_MS);
    }
}