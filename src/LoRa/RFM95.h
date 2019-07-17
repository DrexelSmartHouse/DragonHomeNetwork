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
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 0

class RFM95 : private RH_RF95
{
public:

    RFM95();

    // function prototypes
    void publishSensorReading();
    void initRadio(void);
    void sendMessage(char *message, uint8_t length);
    void waitForMessage();

    /**************************************************************
     * Function: publishLogMsg
     * ------------------------------------------------------------ 
     * summary: sends log messages over serial
     * parameters: string msg
     * return: void
     **************************************************************/
    void publishLogMsg(String msg);

private:
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
    void blink(byte PIN, byte DELAY_MS, byte loops);
};