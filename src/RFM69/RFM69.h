#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF69.h>

#if defined(__AVR_ATmega32U4__) // Feather 32u4 w/Radio
#define RFM69_CS 8
#define RFM69_INT 7
#define RFM69_RST 4
#define LED 13
#define radio() rf69(RFM69_CS, RFM69_INT)
#else
#define RFM69_RST 0
#define LED 13
#define radio() rf69
#endif

#define RF69_FREQ 915.0
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 0

class RFM69 : private RH_RF69
{
public:
    RFM69();

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