/**
 * @file SMU-Communication-Backend.h
 * @brief The header file for SMU-Communication-Backend related stuff.
 * @author Jonas Merkle [JJM] <a href="mailto:jonas.merkle@tam-onestone.net">jonas.merkle@tam-onestone.net</a>
 * @author Dominik Authaler <a href="mailto:dominik.authaler@team-onestone.net">dominik.authaler@team-onestone.net</a>
 * @author
 * This library is maintained by <a href="https://team-onestone.net">Team Onestone</a>.
 * E-Mail: <a href="mailto:info@team-onestone.net">info@team-onestone.net</a>
 * @version 1.0.0
 * @date 09 January 2020
 * @copyright This project is released under the GNU General Public License v3.0
 */

#ifndef SMU_COM_BACKEND_H
#define SMU_COM_BACKEND_H

//////////////
// Includes //
//////////////

// basic Includes
#include <inttypes.h>
#include "Arduino.h"


/////////////
// Defines //
/////////////

#define _SMU_COM_BACKEND_PREAMBEL_SING '~'
#define _SMU_COM_BACKEND_END_SING '#'
#define _SMU_COM_BACKEND_MAX_PAYLOAD_SIZE 25
#define _SMU_COM_BACKEND_TOTAL_SIZE_OFFSET 3
#define _SMU_COM_BACKEND_BAUD_RATE 115200
#define _SMU_COM_BACKEND_SERIAL_INTERFACE Serial3


/////////////
// Members //
/////////////

/**
 * @namespace SMU_Com_Backend
 * 
 * @brief Namespace for SMU-Communication-Backend related stuff.
 */
namespace SMU_Com_Backend
{
	///////////
	// enums //
	///////////

	/**
	 * @enum MessageType
	 * 
	 * @brief Enum that represents the differnet Messages Types for the serial communication.
	 */ 
	enum MessageType : int16_t {
		NONE = 0x00,

		ACK_FAULT = 0x01,
		ACK = 0x02,

		PONG = 0x03,
		GET_STATUS = 0x04,
		GET_ERROR = 0x05,
		RESET = 0x06,

		INIT_SENSOR = 0x1e,
		SET_ACTIVE = 0x1f,

		AUTO_UPDATE = 0x46,
		MAN_UPDATE = 0x47
	};

	/////////////
	// classes //
	/////////////

	/**
	 * @class Message
	 * 
	 * @brief Represents a serial for or from the SMU.
	 */
	class Message {
		// Begin PUBLIC ------------------------------------------------------------------
		public:

			// Constructors

			Message();
			Message(MessageType msgType);
			Message(MessageType msgType, uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize);
			~Message();

			
			// Setter
			void setMsgType(MessageType msgType);
			bool setPayload(uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize);
			void setChecksum(uint8_t checksum);


			// Getters

			MessageType getMsgType();

			uint8_t* getPayload();
			uint8_t getPayloadSize();

			uint8_t getTotalSize();
			uint8_t getChecksum();


		// End PUBLIC --------------------------------------------------------------------

		// Begin PRIVATE -----------------------------------------------------------------
		private:
			MessageType _MsgType;

			uint8_t _payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE];
			uint8_t _payloadSize; 

			uint8_t _checksum;

			void _updateChecksum();
			void _setPayloadZero();

		// End PRIVATE -------------------------------------------------------------------
	};


	///////////////
	// variables //
	///////////////

	static uint16_t serialComTimeout = 50;
	//static HardwareSerial serialInterface = Serial3;


	///////////////
	// functions //
	///////////////

	uint8_t genCheckSum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25]);

	bool checkChecksum(Message* msg);
	bool checkChecksum(Message* msg, uint8_t checksum);
	bool checkChecksum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25], uint8_t checksum);

	void sendMessage(Message* msg);

	bool readNextMessage(Message* msg);

}

#endif