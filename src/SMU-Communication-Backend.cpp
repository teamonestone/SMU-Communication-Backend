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

Message::Message() {
    _MsgType = MessageType::NONE;
    _payload = nullptr;
    _payloadSize = 0;
    _totalSize = 0;

    _checksum = 0;
}

Message::Message(MessageType msgType) {
    _MsgType = msgType;
    _payload = nullptr;
    _payloadSize = 0;
    _totalSize = 2;

    _genChecksum();
}

Message::Message(MessageType msgType, uint8_t* payload, uint8_t payloadSize) {
    _MsgType = msgType;
    _payload = payload;
    _payloadSize = payloadSize;
    _totalSize = _payloadSize + 2;

    _genChecksum();
}


void Message::setMsgType(MessageType msgType) {
    _MsgType = msgType;

    // update checksum
    _genChecksum();
}

bool Message::setPayload(uint8_t* payload, uint8_t payloadSize) {
    // payload size check
    if (payloadSize > 250) {
        return false;
    }

    _payload = payload;
    _payloadSize = _payloadSize;
    _totalSize = _payloadSize + 2;

    // update checksum
    _genChecksum();

    return true;
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
    return _totalSize;
}

uint8_t Message::getChecksum() {
    return _checksum;
}


void Message::_genChecksum() {
    uint32_t tempSum = 0; 

    // add MsgType to checksum
    tempSum += static_cast<int>(_MsgType);

    // add payload to checksum
    for (uint8_t i = 0; i < _payloadSize; i++) {        
        tempSum += static_cast<int>(_payload[i]);
    }

    // % checksum 
    while (tempSum > 255) {
        tempSum = tempSum % _totalSize;
    }

    // set checksum
    _checksum = static_cast<uint8_t>(tempSum)
}


bool checkChecksum(Message* msg) {
    // calc actual checksum
    uint32_t tempSum = 0; 

    // add MsgType to checksum
    tempSum += static_cast<int>(msg->getMsgType());

    // add payload to checksum
    for (uint8_t i = 0; i < msg->getPayloadSize(); i++) {        
        tempSum += static_cast<int>((msg->getPayload())[i]);
    }

    // % checksum 
    while (tempSum > 255) {
        tempSum = tempSum % msg->getTotalSize();
    }

    // compare checksum
    (msg->getChecksum() == tempSum) ? return true : return false; 
}