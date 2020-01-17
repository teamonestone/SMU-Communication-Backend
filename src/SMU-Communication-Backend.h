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
#include "Arduino.h"
#include <inttypes.h>


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
 * @namespace smu_com_backend
 * 
 * @brief Namespace for SMU-Communication-Backend related stuff.
 */
namespace smu_com_backend
{
	///////////
	// enums //
	///////////

	/**
	 * @enum MessageType
	 * 
	 * @brief Enum that represents the differnet Messages Types for the serial communication.
	 */ 
	enum MessageType : int8_t {
		ERROR = -1,

		NONE = 0x00,

		ACK_FAULT = 0x01,
		ACK = 0x02,

		PONG = 0x03,
		G_STATUS = 0x04,
		G_COM_ERROR = 0x05,
		G_SMU_ERROR = 0x06,

		RESET = 0x09,

		FIRMWARE_V = 0x0a,
		COM_BACK_V = 0x0b,

		INIT_SENSOR = 0x1e,
		S_ACTIVE = 0x1f,
		G_ACTIVE = 0x20,

		S_AUTO_UPDATE = 0x46,
		G_AUTO_UPDATE = 0x47, 
		MAN_UPDATE = 0x48,

		READ_SENSOR = 0x64
	};

	/**
	 * @enum ComErrorInfo
	 * 
	 * @brief Enum that represents the differnet communication error informations.
	 */ 
	enum ComErrorInfo : uint8_t {
		NO_COM_ERROR = 0x00,
		
		NO_START_SIGN = 0x01,
		NO_END_SING = 0x02,
		INV_PAYL_SIZE = 0x03,
		INV_CHECKSUM = 0x04,
		NOT_ENOUGH_DATA = 0x05,
		REC_TIMEOUT = 0x06
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

			// util
			void clear();

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
	static ComErrorInfo lastComErrorInfo = ComErrorInfo::NO_COM_ERROR;
	uint16_t const _libVersion = 100;


	///////////////
	// functions //
	///////////////

	uint8_t genCheckSum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25]);

	bool checkChecksum(Message* msg);
	bool checkChecksum(Message* msg, uint8_t checksum);
	bool checkChecksum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25], uint8_t checksum);

	void sendMessage(Message* msg);

	bool readNextMessage(Message* msg);

	uint16_t getVersion(); 
}

#endif