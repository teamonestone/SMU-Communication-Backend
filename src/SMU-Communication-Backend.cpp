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

using namespace SMU_Com_Backend;

///////////////////////
// class construcots //
///////////////////////

Message::Message() {
    _MsgType = MessageType::NONE;
    _payloadSize = 0;

	_setPayloadZero();
	
    _checksum = 0;
}

Message::Message(MessageType msgType) {
    _MsgType = msgType;
    _payloadSize = 0;

	_setPayloadZero();
	
    _setChecksum();
}

Message::Message(MessageType msgType, uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize) {
    _MsgType = msgType;
    _payloadSize = payloadSize;
	
	_setPayloadZero();

    // copy payload
    for (uint8_t i = 0; i < _payloadSize; i++) {
        _payload[i] = payload[i];
    }

    _setChecksum();
}

Message::~Message(){
    
}

/////////////////////
// class functions //
/////////////////////

void Message::setMsgType(MessageType msgType) {
    _MsgType = msgType;

    // update checksum
    _setChecksum();
}

bool Message::setPayload(uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize) {
    // payload size check
    if (payloadSize > _SMU_COM_BACKEND_MAX_PAYLOAD_SIZE) {
        return false;
    }

    _payloadSize = _payloadSize;

    // copy payload
    for (uint8_t i = 0; i < _payloadSize; i++) {
        _payload[i] = payload[i];
    }

    // update checksum
    _setChecksum();

    return true;
}

void Message::setChecksum(uint8_t checksum) {
    _checksum = checksum;
}

MessageType Message::getMsgType() {
    return _MsgType;
}

uint8_t* Message::getPayload() {
    return _payload;
}

uint8_t Message::getPayloadSize() {
    return _payloadSize;
}

uint8_t Message::getTotalSize() {
    return _payloadSize + _SMU_COM_BACKEND_TOTAL_SIZE_OFFSET;
}

uint8_t Message::getChecksum() {
    return _checksum;
}

void Message::_setChecksum() {
    // set checksum
    _checksum = genCheckSum(getMsgType(), getPayloadSize(), getPayload());
}

void Message::_setPayloadZero() {
	for (uint8_t i = 0; i < _SMU_COM_BACKEND_MAX_PAYLOAD_SIZE; i++) {
		_payload[i] = 0;
	}
}

/////////////////////////
// namespace functions // 
/////////////////////////

uint8_t SMU_Com_Backend::genCheckSum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25]) {
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

bool SMU_Com_Backend::checkChecksum(Message* msg) {
    if (genCheckSum(msg->getMsgType(), msg->getPayloadSize(), msg->getPayload()) == msg->getChecksum()) {
        return true;
    }
    else {
        return false;
    }
}

bool SMU_Com_Backend::checkChecksum(Message* msg, uint8_t checksum) {
    if (genCheckSum(msg->getMsgType(), msg->getPayloadSize(), msg->getPayload()) == checksum) {
        return true;
    }
    else {
        return false;
    }
}

bool SMU_Com_Backend::checkChecksum(MessageType msgType, uint8_t payloadSize, uint8_t payload[25], uint8_t checksum) {
    if (genCheckSum(msgType, payloadSize, payload) == checksum) {
        return true;
    }
    else {
        return false;
    }
}

void SMU_Com_Backend::sendMessage(Message* msg) {
    // write preambel
    serialInterface.write(_SMU_COM_BACKEND_PREAMBEL_SING);

    // write message type 
    serialInterface.write(msg->getPayloadSize());

    // write message type 
    serialInterface.write(msg->getMsgType());

    // write payload
    serialInterface.write(msg->getPayload(), msg->getPayloadSize());

    // write end-sign
    serialInterface.write(_SMU_COM_BACKEND_END_SING);
}

bool SMU_Com_Backend::readNextMessage(Message* msg) {
    // check if enought bytes have been received
    if (serialInterface.available() < 5) {
        msg->setMsgType(MessageType::NONE);
        return false;
    }

    // timeout stuff
    uint32_t tempForTime = millis();

    // find preambel
    while (true) {
        // check timeout & availability
        if (serialInterface.available() <= 0 || tempForTime + serialComTimeout < millis()) {
            msg->setMsgType(MessageType::NONE);
            return false;
        }
        
        // read byte & check 
        if(serialInterface.read() == _SMU_COM_BACKEND_PREAMBEL_SING) {
            break;
        }
    }

    // read message type
    if (serialInterface.available() <= 4) {
        msg->setMsgType(MessageType::NONE);
        return false;
    }
    msg->setMsgType(static_cast<MessageType>(serialInterface.read()));
    
    // read payload size
    uint8_t payloadSize = serialInterface.read();

    // read payload & checksum
    uint8_t counter = 0;
    uint8_t payload[25] = {0};
    tempForTime = millis();
    while (true) {
        // check timeout
        if (tempForTime + serialComTimeout < millis()) {
            msg->setMsgType(MessageType::NONE);
            return false;
        }
        
        // check availability
        if (serialInterface.available() > 0) {
            uint8_t b = serialInterface.read();
            
            counter++;

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
                        return false;
                    }
                }
                else {
                    msg->setMsgType(MessageType::NONE);
                    return false;
                }
            }
        }
    }
}