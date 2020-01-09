#include "SMU-Communication-Backend.h"

using namespace SMU_Com_Backend;

void setup() {
	// init serial-com
	Serial.begin(9600);
	Serial1.begin(9600)
}

void loop() {

	Serial.println("---------------------------------------------------");

	// generate random message
	Serial.println("Generate Payload...");
	uint8_t payload[25] = {0};
	for (uint8_t i = 0; i < 25; i++) {
		payload[i] = static_cast<uint8_t>(random(255));
		Serial.print("    [");
		Serial.print(i);
		Serial.print("]: ");
		Serial.println(payload[i]);
	}
	Serial.println("done!");
	Message msg(MessageType::PING, payload, 25);

	// checksum test
	Serial.print("Checksum Check: ");
	Serial.println(checkChecksum(&msg));
	
	// send message
	Serial.println("Send Message... ");
	sendMessage(&Serial1, &msg);
	Serial.println("done!")

	Serial.println();

	// read message
	Message msgRd;
	Serial.print("Read Message: ");
	Serial.println(readNextMessage(&Serial1, *msgRd));
	

	Serial.println("---------------------------------------------------");
}
