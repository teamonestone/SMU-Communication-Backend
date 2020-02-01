/**
 * @file SMU-Communication-Backend.cpp
 * @brief The source file for SMU-Communication-Backend related stuff.
 * @author Jonas Merkle [JJM] <a href="mailto:jonas.merkle@tam-onestone.net">jonas.merkle@tam-onestone.net</a>
 * @author Dominik Authaler <a href="mailto:dominik.authaler@team-onestone.net">dominik.authaler@team-onestone.net</a>
 * @author
 * This library is maintained by <a href="https://team-onestone.net">Team Onestone</a>.
 * E-Mail: <a href="mailto:info@team-onestone.net">info@team-onestone.net</a>
 * @version 1.0.0
 * @date 09 January 2020
 * @copyright This project is released under the GNU General Public License v3.0
 */

// associated header
#include "SMU-Communication-Backend.h"

// defines
#define _SMU_COM_BACKEND_LIB_VERSION 10000

// used namespaces
using namespace smu_com_backend;


///////////////////////
// class construcots //
///////////////////////

/**
 * @brief Default Constructor of the Message class.
 * 
 */
Message::Message() {
    _MsgType = MessageType::NONE;
    _payloadSize = 0;

	_setPayloadZero();
	
    _checksum = 0;
}

/**
 * @brief Empty Message Construcotr.
 * 
 * @param The message type of the message.
 */
Message::Message(MessageType msgType) {
    _MsgType = msgType;
    _payloadSize = 0;

	_setPayloadZero();
	
    _updateChecksum();
}

/**
 * @brief Full Constructor.
 * 
 * @param msgType The message type of the message.
 * @param payload Pointer to an array with up to 25 bytes of payload.
 * @param payloadSize The size of the payload in bytes. 
 */
Message::Message(MessageType msgType, uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize) {
    _MsgType = msgType;
    _payloadSize = payloadSize;
	
	_setPayloadZero();

    // copy payload
    for (uint8_t i = 0; i < _payloadSize; i++) {
        _payload[i] = payload[i];
    }

    _updateChecksum();
}

/**
 * @brief Default Destructor of the Message class.
 *  
 */
Message::~Message() {
    
}


/////////////////////
// class functions //
/////////////////////

/**
 * @brief Set the new message type of a message.
 * 
 * @param msgType the new message type.
 */
void Message::setMsgType(MessageType msgType) {
    _MsgType = msgType;

    // update checksum
    _updateChecksum();
}

/**
 * @brief Set the new payload of a message.
 * 
 * @param payload Pointer to an array with up to 25 bytes of new payload.
 * @param payloadSize The size of the new payload in bytes. 
 * 
 * @return true on success, else false.
 */
bool Message::setPayload(uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize) {
    // payload size check
    if (payloadSize > _SMU_COM_BACKEND_MAX_PAYLOAD_SIZE) {
        return false;
    }

	_setPayloadZero();
    _payloadSize = payloadSize;

    // copy payload
    for (uint8_t i = 0; i < _payloadSize; i++) {
        _payload[i] = payload[i];
    }

    // update checksum
    _updateChecksum();

    return true;
}

/**
 * @brief Set the new checksum of a message.
 * 
 * @param checksum The new checksum.
 */
void Message::setChecksum(uint8_t checksum) {
    _checksum = checksum;
}

/**
 * @brief Get the message type ot a message.
 * 
 * @return The message type as SUM_Com_Backend::MessageType.
 */
MessageType Message::getMsgType() {
    return _MsgType;
}

/**
 * @brief Get the Pointer to the payload of a message.
 * 
 * @return The pointer to the payload as uint8_t*.
 */
uint8_t* Message::getPayload() {
    return _payload;
}

/**
 * @brief Get the size of the payload of a message.
 * 
 * @return The size of the payload in bytes as uint8_t.
 */
uint8_t Message::getPayloadSize() {
    return _payloadSize;
}

/**
 * @brief Get the totla size of a message.
 * 
 * @return The total size of the message in bytes as uint8_t.
 */
uint8_t Message::getTotalSize() {
    return _payloadSize + _SMU_COM_BACKEND_TOTAL_SIZE_OFFSET;
}

/**
 * @brief Get the checksum of a message.
 * 
 * @return The checksum of the message as uint8_t.
 */
uint8_t Message::getChecksum() {
    return _checksum;
}

/**
 * @brief Clear the content of a message.
 * 
 */
void Message::clear() {
    _setPayloadZero();
    _payloadSize = 0;
    _checksum = 0;
}

/**
 * @brief Updates the checksum of an message.
 * 
 */
void Message::_updateChecksum() {
    // set checksum
    _checksum = genCheckSum(getMsgType(), getPayloadSize(), getPayload());
}

/**
 * @brief Sets the payload of an message to all zeros.
 * 
 */
void Message::_setPayloadZero() {
	for (uint8_t i = 0; i < _SMU_COM_BACKEND_MAX_PAYLOAD_SIZE; i++) {
		_payload[i] = 0;
	}
}


/////////////////////////
// namespace functions // 
/////////////////////////

/**
 * @brief Calculates a Checksum.
 * 
 * @param msgType The message type of the message.
 * @param payloadSize The size of the payload in bytes. 
 * @param payload Pointer to an array with up to 25 bytes of payload.
 * 
 * @return The caluculated checksum as uint8_t.
 */
uint8_t smu_com_backend::genCheckSum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25]) {
    // calc actual checksum
    uint16_t tempSum = 0; 

    // add MsgType to checksum
    tempSum += static_cast<int16_t>(msgType);

    // add payload size to checksum
    tempSum += static_cast<int16_t>(payloadSize);

    // add payload to checksum
    for (uint8_t i = 0; i < payloadSize; i++) {        
        tempSum += static_cast<int16_t>(payload[i]);
    }

    // % checksum 
    while (tempSum > 255) {
        tempSum = tempSum % (payloadSize + _SMU_COM_BACKEND_TOTAL_SIZE_OFFSET);
    }

    // compare checksum
    return static_cast<uint8_t>(tempSum);
}

/**
 * @brief Checks if the checksum that is stored in a message is correct.
 * 
 * @param msg A pointer to the message which should be checked as Message*.
 * 
 * @return True if checksum is correct, else false.
 */
bool smu_com_backend::checkChecksum(Message* msg) {
	if (genCheckSum(msg->getMsgType(), msg->getPayloadSize(), msg->getPayload()) == msg->getChecksum()) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Check is the claculated checksum and a other checksum are equal.
 * 
 * @param msg Pointer to a message as Message*.
 * @param checksum A given checksum as uint8_t.
 * 
 * @return True if both are equal, else false.
 */
bool smu_com_backend::checkChecksum(Message* msg, uint8_t checksum) {
    if (genCheckSum(msg->getMsgType(), msg->getPayloadSize(), msg->getPayload()) == checksum) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Checks if the checksum of given data is equal to another checksum.
 * 
 * @param msgType The message type..
 * @param payloadSize The size of the payload in bytes. 
 * @param payload Pointer to an array with up to 25 bytes of payload.
 * @param checksum  A given checksum as uint8_t.
 * 
 * @return True if both are equal, else false.
 */
bool smu_com_backend::checkChecksum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25], uint8_t checksum) {
    if (genCheckSum(msgType, payloadSize, payload) == checksum) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Sends message over a defined Serial interface.
 * 
 * @param msg Pointer to a message as Message*.
 */
void smu_com_backend::sendMessage(Message* msg) {
	// init serial-com
	_SMU_COM_BACKEND_SERIAL_INTERFACE.begin(_SMU_COM_BACKEND_BAUD_RATE);
	
    // write preambel
    _SMU_COM_BACKEND_SERIAL_INTERFACE.write(_SMU_COM_BACKEND_PREAMBEL_SING);

    // write message type 
    _SMU_COM_BACKEND_SERIAL_INTERFACE.write(msg->getMsgType());

	// write payload size
    _SMU_COM_BACKEND_SERIAL_INTERFACE.write(msg->getPayloadSize());

    // write payload
	for (uint8_t i = 0; i < msg->getPayloadSize(); i++) {
		_SMU_COM_BACKEND_SERIAL_INTERFACE.write((msg->getPayload())[i]);
	}
	
	// write checksum
    _SMU_COM_BACKEND_SERIAL_INTERFACE.write(msg->getChecksum());

    // write end-sign
    _SMU_COM_BACKEND_SERIAL_INTERFACE.write(_SMU_COM_BACKEND_END_SING);
	
	// flush serial port
	_SMU_COM_BACKEND_SERIAL_INTERFACE.flush();
}

/**
 * @brief Reads the next message out of the serial buffer if one is available.
 * 
 * @param msg  A pointer to a Message object where the received message should be stored.
 * 
 * @return True on success, else false.
 */
bool smu_com_backend::readNextMessage(Message* msg) {
	uint32_t tempForTime = 0;
	uint8_t payloadSize = 0;
    uint8_t counter = 0;
    uint8_t payload[25] = {0};
	
	// init serial-com
	_SMU_COM_BACKEND_SERIAL_INTERFACE.begin(_SMU_COM_BACKEND_BAUD_RATE);
	
    // check if enought bytes have been received
    if (_SMU_COM_BACKEND_SERIAL_INTERFACE.available() < 5) {
        msg->setMsgType(MessageType::NONE);
        lastComErrorInfo = ComErrorInfo::NO_COM_ERROR;
        return false;
    }

    // timeout stuff
    tempForTime = millis();

    // find preambel
    while (true) {
        // check timeout & availability
        if (_SMU_COM_BACKEND_SERIAL_INTERFACE.available() <= 0 || tempForTime + serialComTimeout < millis()) {
            MessageType t = msg->getMsgType();
            uint8_t p[2] = {0};
            msg->setMsgType(MessageType::ERROR);
            p[0] = static_cast<uint8_t>(t);
            p[1] = static_cast<uint8_t>(ComErrorInfo::NO_START_SIGN);
            msg->setPayload(p, 2);
            lastComErrorInfo = ComErrorInfo::NO_START_SIGN;
            return false;
        }
        
        // read byte & check 
        if(_SMU_COM_BACKEND_SERIAL_INTERFACE.read() == _SMU_COM_BACKEND_PREAMBEL_SING) {
            break;
        }
    }

    // read message type
    if (_SMU_COM_BACKEND_SERIAL_INTERFACE.available() <= _SMU_COM_BACKEND_MIN_MSG_LENGHT) {  // 4 or more bytes available.
        uint8_t p[2] = {0};
        msg->setMsgType(MessageType::ERROR);
        p[0] = static_cast<uint8_t>(MessageType::NONE);
        p[1] = static_cast<uint8_t>(ComErrorInfo::NOT_ENOUGH_DATA);
        msg->setPayload(p, 2);
        lastComErrorInfo = ComErrorInfo::NOT_ENOUGH_DATA;
        return false;
    }
    msg->setMsgType(static_cast<MessageType>(_SMU_COM_BACKEND_SERIAL_INTERFACE.read()));
    
    // read payload size
    payloadSize = static_cast<uint8_t>(_SMU_COM_BACKEND_SERIAL_INTERFACE.read());

    // read payload & checksum
    tempForTime = millis();
    while (true) {
        // check timeout
        if (tempForTime + serialComTimeout < millis()) {
            MessageType t = msg->getMsgType();
            uint8_t p[2] = {0};
            msg->setMsgType(MessageType::ERROR);
            p[0] = static_cast<uint8_t>(t);
            p[1] = static_cast<uint8_t>(ComErrorInfo::REC_TIMEOUT);
            msg->setPayload(p, 2);
            lastComErrorInfo = ComErrorInfo::REC_TIMEOUT;
            return false;
        }
        
        // check availability
        if (_SMU_COM_BACKEND_SERIAL_INTERFACE.available() > 0) {
            uint8_t b = _SMU_COM_BACKEND_SERIAL_INTERFACE.read();

            if (counter < payloadSize) {
                // reading payload part
                payload[counter] = b;
            }
            else if (counter == payloadSize){
                // reading checksum part
                msg->setPayload(payload, payloadSize);
                msg->setChecksum(b);
            }
            else {
                // check for end-sing
                if (b ==  _SMU_COM_BACKEND_END_SING) {
                    // check checksum
                    if (checkChecksum(msg)) {
                        return true;
                    }
                    else {
                        MessageType t = msg->getMsgType();
                        uint8_t p[2] = {0};
                        msg->setMsgType(MessageType::ERROR);
                        p[0] = static_cast<uint8_t>(t);
                        p[1] = static_cast<uint8_t>(ComErrorInfo::INV_CHECKSUM);
                        msg->setPayload(p, 2);
                        lastComErrorInfo = ComErrorInfo::INV_CHECKSUM;
                        return false;
                    }
                }
                else {
                    MessageType t = msg->getMsgType();
                    uint8_t p[2] = {0};
                    msg->setMsgType(MessageType::ERROR);
                    p[0] = static_cast<uint8_t>(t);
                    p[1] = static_cast<uint8_t>(ComErrorInfo::NO_END_SING);
                    msg->setPayload(p, 2);
                    lastComErrorInfo = ComErrorInfo::NO_END_SING;
                    return false;
                }
            }
			
			counter++;
        }
    }
}

/**
 * @brief Get the version of the library.
 * 
 * @retrun the current version of the library.
 */
uint16_t smu_com_backend::getVersion() {
    return _SMU_COM_BACKEND_LIB_VERSION;
}