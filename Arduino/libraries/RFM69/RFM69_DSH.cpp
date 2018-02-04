#include <RFM69_DSH.h>

#include <RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>

volatile uint8_t RFM69_DSH::DATA_REQUESTED;
volatile uint8_t RFM69_DSH::END_RECEIVED;
volatile uint8_t RFM69_DSH::EVENT_RECEIVED;
volatile uint8_t RFM69_DSH::STR_PACKET_RECEIVED;
volatile uint8_t RFM69_DSH::SENSOR_DATA_PACKET_RECEIVED;
volatile uint8_t RFM69_DSH::ERROR_RECEIVED;


void RFM69_DSH::popSensorReading() {
	// check to make sure the packet is the right length
	if (DATALEN != sizeof(SensorReading)) {
		receiveFail();
		return;
	}

	// read in all of the bytes of the sensor reading
	for (uint8_t i = 0; i < DATALEN; i++) {
		DATA[i] = SPI.transfer(0);
	}

	// adjust the DATALEN to the new size
	// NOTE: DATA will be invalid after this function returns
	// see RFM69::interruptHandler();
	DATALEN -= sizeof(SensorReading);

	// cast byte array into struct
	SENSOR_READING = *((SensorReading*)DATA);

}

void RFM69_DSH::popStrPacket() {

	// read in all of the bytes of the string
	for (uint8_t i = 0; i < DATALEN; i++) {
		DATA[i] = SPI.transfer(0);
	}

	// set the char at the end to null
	// just in case the sender forgot
	// or it was cut off
	DATA[DATALEN-1] = '\0';

	// adjust the DATALEN to the new size
	// NOTE: DATA will be invalid after this function returns
	// see RFM69::interruptHandler();
	DATALEN = 0;

	RECEIVED_STR = String((const char*)DATA);

}

void RFM69_DSH::receiveFail() {
	// for now dont send ack
	// TODO: send an error back to the gateway
	ACK_REQUESTED = 0;

	// dsh flags
	DATA_REQUESTED = 0;
	END_RECEIVED = 0;
	EVENT_RECEIVED = 0;
	STR_PACKET_RECEIVED = 0;
	SENSOR_DATA_PACKET_RECEIVED = 0;
	ERROR_RECEIVED = 0;

}

bool RFM69_DSH::sendSensorReading(const String& sensorType, float data, uint8_t receiverId=GATEWAY_ID) {

	//convert string into char array
	char senTypeCharArr[MAX_SENSOR_TYPE_LEN];
	sensorType.toCharArray(senTypeCharArr, MAX_SENSOR_TYPE_LEN);

	//create a local SensorReading
	SensorReading senRead;
	strncpy(senRead.sensorType, senTypeCharArr, strlen(senTypeCharArr)+1);
	senRead.data = data;

	//Serial.print("The sensor type:");
	//Serial.println(senRead.sensorType);

	// atmept to send and return the result
	return RFM69_DSH::sendWithRetry(receiverId, (const void*)(&senRead), sizeof(SensorReading), RFM69_CTL_SEN_DATA_PACKET, 1);

}

bool RFM69_DSH::sendEnd(uint8_t receiverId=GATEWAY_ID) {
	return RFM69_DSH::sendWithRetry(receiverId, NULL, 0, RFM69_CTL_SEND_END);
}

bool RFM69_DSH::sendError(const String& errorMsg, uint8_t receiverId=GATEWAY_ID) {
	return RFM69_DSH::sendString(errorMsg, receiverId, RFM69_CTL_ERROR);
}

bool RFM69_DSH::sendStrEvent(const String& msg, uint8_t receiverId=GATEWAY_ID) {
	return RFM69_DSH::sendString(msg, receiverId, RFM69_CTL_EVENT);
}

bool RFM69_DSH::sendString(const String& str, uint8_t receiverId, uint8_t CTLbyte=RFM69_CTL_STR_PACKET) {

	unsigned int c_str_length = str.length()+1;

	// trim the string if it is too large
	if (c_str_length > RF69_MAX_DATA_LEN) {
		str.substring(0, RF69_MAX_DATA_LEN-1);
	}

	return RFM69_DSH::sendWithRetry(receiverId, (const void*) str.c_str(), str.length()+1, CTLbyte | RFM69_CTL_STR_PACKET);
}

bool RFM69_DSH::requestAll(uint8_t nodeId) {
	return RFM69_DSH::sendWithRetry(nodeId, NULL, 0, RFM69_CTL_DATA_REQ);
}

bool RFM69_DSH::request(const String& sensorType, uint8_t nodeId) {
	return RFM69_DSH::sendString(sensorType, nodeId, RFM69_CTL_DATA_REQ);
}

// simple message with no data but request an ack
bool RFM69_DSH::ping(uint8_t nodeId) {
	// use the base class version
	return RFM69::sendWithRetry(nodeId, NULL, 0, 1);
}

bool RFM69_DSH::sendWithRetry(uint8_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t CTLbyte, uint8_t retries=0, uint8_t retryWaitTime=80) {

  	uint32_t sentTime;

  	// add the req ack bit
  	CTLbyte = CTLbyte | RFM69_CTL_REQACK;

  	for (uint8_t i = 0; i <= retries; i++) {

		RFM69_DSH::send(toAddress, buffer, bufferSize, CTLbyte);
    	sentTime = millis();

    	while (millis() - sentTime < retryWaitTime) {

      		if (ACKReceived(toAddress)) {
        		//Serial.print(" ~ms:"); Serial.print(millis() - sentTime);
        		return true;
      		}

    	}
    	//Serial.print(" RETRY#"); Serial.println(i + 1);

  	}

  	return false;

}

void RFM69_DSH::send(uint8_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t CTLbyte) {

 	writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  	uint32_t now = millis();
  	while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) receiveDone();
  	RFM69_DSH::sendFrame(toAddress, buffer, bufferSize, CTLbyte);

}

//internal send function that accepts a CTLbyte
void RFM69_DSH::sendFrame(uint8_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t CTLbyte) {

	// most of this is the original function except
	// that the CTL byte is now an input

	setMode(RF69_MODE_STANDBY); // turn off receiver to prevent reception while filling fifo
  	while ((readReg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00); // wait for ModeReady
  	writeReg(REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00); // DIO0 is "Packet Sent"
  	if (bufferSize > RF69_MAX_DATA_LEN) bufferSize = RF69_MAX_DATA_LEN;

  	// write to FIFO
  	select();
  	SPI.transfer(REG_FIFO | 0x80);
  	SPI.transfer(bufferSize + 3);
  	SPI.transfer(toAddress);
  	SPI.transfer(_address);
  	SPI.transfer(CTLbyte);

  	for (uint8_t i = 0; i < bufferSize; i++)
    	SPI.transfer(((uint8_t*) buffer)[i]);
  	unselect();

  	// no need to wait for transmit mode to be ready since its handled by the radio
  	setMode(RF69_MODE_TX);
  	uint32_t txStart = millis();
  	while (digitalRead(_interruptPin) == 0 && millis() - txStart < RF69_TX_LIMIT_MS); // wait for DIO0 to turn HIGH signalling transmission finish
  	//while (readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_PACKETSENT == 0x00); // wait for ModeReady
  	setMode(RF69_MODE_STANDBY);
}


// method is called when a new packet is received
void RFM69_DSH::interruptHook(uint8_t CTLbyte) {

	// the first two bits are handled by the lib
	// these are the ack received and requested bits

	// set all the flags based on the CTL byte
	DATA_REQUESTED = CTLbyte & RFM69_CTL_DATA_REQ;
	END_RECEIVED   = CTLbyte & RFM69_CTL_SEND_END;
	EVENT_RECEIVED = CTLbyte & RFM69_CTL_EVENT;
	STR_PACKET_RECEIVED = CTLbyte & RFM69_CTL_STR_PACKET;
	SENSOR_DATA_PACKET_RECEIVED = CTLbyte & RFM69_CTL_SEN_DATA_PACKET;
	ERROR_RECEIVED = STR_PACKET_RECEIVED && SENSOR_DATA_PACKET_RECEIVED;

	if (STR_PACKET_RECEIVED) popStrPacket();
	else if (SENSOR_DATA_PACKET_RECEIVED) popSensorReading();

}
