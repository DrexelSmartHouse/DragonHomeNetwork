#include "RFM69.h"

// Singleton instance of the radio driver. Managed by preprocessor directives
RH_RF69 radio();

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(rf69, SERVER_ADDRESS);

RFM69::RFM69()
{
}

void RFM69::initRadio()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    pinMode(RFM69_RST, OUTPUT);
    digitalWrite(RFM69_RST, LOW);

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
    if (!manager.init())
        Serial.println("init failed");

    if (!rf69.setFrequency(RF69_FREQ))
        Serial.println("setFrequency failed");

    rf69.setTxPower(14, true);

    // The encryption key has to be the same as the one in the server
    uint8_t key[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    rf69.setEncryptionKey(key);
}

void RFM69::waitForMessage()
{
    uint8_t data[] = "Acknowledged";
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    char *vals;

    if (manager.available())
    {
        // Wait for a message addressed to us from the client
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (manager.recvfromAck(buf, &len, &from))
        {
            vals = strtok((char *)buf, ",:");
            // For every sensor type sent.
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

            Serial.print("DHN/");
            Serial.print(SERVER_ADDRESS, DEC);
            Serial.print('/');
            Serial.print(from, DEC);
            Serial.print('/');
            Serial.print("RSSI/");
            Serial.print(rf69.lastRssi(), DEC);
            Serial.print('\n');
            // Send a reply back to the originator client
            if (!manager.sendtoWait(data, sizeof(data), from))
                Serial.println("Ack failed");
        }
    }
}

// TODO: Make function return boolean on pass/fail
void RFM69::sendMessage(char *message, uint8_t length)
{
    if (sizeof(message) > RH_RF69_MAX_MESSAGE_LEN)
    {
        Serial.println("Sending failed. Message too large for RFM69\n");
    }

    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

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

void RFM69::publishLogMsg(String msg)
{
    Serial.print(F("/log:"));
    Serial.print(msg);
    Serial.print('\n');
}

void RFM69::blink(byte PIN, byte DELAY_MS, byte loops)
{
    for (byte i = 0; i < loops; i++)
    {
        digitalWrite(PIN, HIGH);
        delay(DELAY_MS);
        digitalWrite(PIN, LOW);
        delay(DELAY_MS);
    }
}