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
    _payload = {0};
    _payloadSize = 0;

    _checksum = 0;
}

Message::Message(MessageType msgType) {
    _MsgType = msgType;
    _payload = {0};
    _payloadSize = 0;

    _genChecksum();
}

Message::Message(MessageType msgType, uint8_t payload[_SMU_COM_BACKEND_MAX_PAYLOAD_SIZE], uint8_t payloadSize) {
    _MsgType = msgType;
    _payload = {0};
    _payloadSize = payloadSize;

    // copy payload
    for (uint8_t i = 0; i < _payloadSize; i++) {
        _payload[i] = payload[i];
    }

    _genChecksum();
}

~Message::Message(){
    
}

/////////////////////
// class functions //
/////////////////////

void Message::setMsgType(MessageType msgType) {
    _MsgType = msgType;

    // update checksum
    _genChecksum();
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
    _genChecksum();

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

void Message::_genChecksum() {
    uint16_t tempSum = 0; 

    // add MsgType to checksum
    tempSum += static_cast<int16_t>(getMsgType());

    // add payload size to checksum
    tempSum += static_cast<int16_t>(getPayloadSize());

    // add payload to checksum
    for (uint8_t i = 0; i < getPayloadSize(); i++) {        
        tempSum += static_cast<int16_t>((getPayload())[i]);
    }

    // % checksum 
    while (tempSum > 255) {
        tempSum = tempSum % getTotalSize();
    }

    // set checksum
    _checksum = static_cast<uint8_t>(tempSum)
}


/////////////////////////
// namespace functions // 
/////////////////////////

bool checkChecksum(Message* msg) {
    // calc actual checksum
    uint16_t tempSum = 0; 

    // add MsgType to checksum
    tempSum += static_cast<int16_t>(msg->getMsgType());

    // add payload size to checksum
    tempSum += static_cast<int16_t>(msg->getPayloadSize()));

    // add payload to checksum
    for (uint8_t i = 0; i < msg->getPayloadSize(); i++) {        
        tempSum += static_cast<int16_t>((msg->getPayload())[i]);
    }

    // % checksum 
    while (tempSum > 255) {
        tempSum = tempSum % msg->getTotalSize();
    }

    // compare checksum
    (msg->getChecksum() == tempSum) ? return true : return false; 
}

void sendMessage(void* SerialComObj, Message* msg) {
    // write preambel
    SerialComObj->write(_SMU_COM_BACKEND_PREAMBEL_SING);

    // write message type 
    SerialComObj->write(msg->getPayloadSize());

    // write message type 
    SerialComObj->write(msg->getMsgType());

    // write payload
    SerialComObj->write(msg->getPayload(), msg->getPayloadSize());

    // write end-sign
    SerialComObj->write(_SMU_COM_BACKEND_END_SING);
}

bool readNextMessage(void* SerialComObj, Message* msg) {
    // check if enought bytes have been received
    if (SerialComObj->available() < 5) {
        msg->getMsgType = MessageType::None;
        return false;
    }

    // timeout stuff
    uint32_t tempForTime = millis();

    // find preambel
    while (true) {
        // check timeout & availability
        if (SerialComObj->available() <= 0 || tempForTime + serialComTimeout < millis()) {
            msg->getMsgType = MessageType::None;
            return false;
        }
        
        // read byte & check 
        if(SerialComObj->read() == _SMU_COM_BACKEND_PREAMBEL_SING) {
            break;
        }
    }

    // read message type
    if (SerialComObj->available() <= 4) {
        msg->getMsgType = MessageType::None;
        return false;
    }
    msg->setMsgType(SerialComObj->read());
    
    // read payload size
    uint8_t payloadSize = SerialComObj->read();

    // read payload & checksum
    uint8_t counter = 0;
    uint8_t payload[25] = {0};
    tempForTime = millis();
    while (true) {
        // check timeout
        if (tempForTime + serialComTimeout < millis()) {
            msg->getMsgType = MessageType::None;
            return false;
        }
        
        // check availability
        if (SerialComObj->available() > 0) {
            uint8_t b = SerialComObj->read();
            
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
                    msg->getMsgType = MessageType::None;
                    return false;
                }
            }
        }
    }
}