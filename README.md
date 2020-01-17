# SMU-Communication-Backend
The Communication Backend for the [Universal-I2C-Sensor-Master-Unit](https://github.com/teamonestone/Universal-I2C-Sensor-Master-Unit).


## General Message Formate

All serial messages used to controll the SMU board follow the following format: 

```
|-----------------|-------------|-------------|----------------------|----------|-----------------|
| '~' as Preambel | MessageType | PayloadSize | 0 - 25 Bytes of Data | Checksum | '#' as End-Sign |
|      1 Char     |   1 Byte    |    1 Byte   |     0 - 25 Bytes     |  1 Byte  |      1 Char     |
|-----------------|-------------|-------------|----------------------|----------|-----------------|
```


## MessageType

| Dec-Code | Hex-Code | MessageType     | Data Bytes                                               | Description                                               |
|----------|----------|-----------------|----------------------------------------------------------|-----------------------------------------------------------|
| -1       | 0xff     | `ERROR`         | additional information (up to 25 bytes)                  | message type to indicate an error                         |
| 0        | 0x00     | `NONE`          | -                                                        | empty message -> no message                               |
| 1        | 0x01     | `ACK_FAULT`     | 8-bit message-type of last rec message + error code      | Indicates a coomunication error                           |
| 2        | 0x02     | `ACK`           | 8-bit message-type that is been acknowledged + payload   | ACK + payload send back (can be up to 24 bytes)           | 
| 3        | 0x03     | `PONG`          | 8-bit random value                                       | Ping Request                                              |
| 4        | 0x04     | `G_STATUS`      | -                                                        | Get the current system stauts code from the SMU           |
| 5        | 0x05     | `G_COM_ERROR`   | -                                                        | Get the current comunication error code from the SMU      |
| 6        | 0x09     | `G_SMU_ERROR`   | -                                                        | Get the current SMU error code from the SMU               |
| 9        | 0x09     | `RESET`         | -                                                        | Perform a soft reset of the SMU                           |
| 10       | 0x0a     | `FIRMWARE_V`    | -                                                        | Request the firmware-version of the SMU                   |
| 11       | 0x0b     | `COM_BACK_V`    | -                                                        | Request the communication backend version of the SMU      |
| 30       | 0x1e     | `INIT_SENSOR`   | 8-bit sensor-type + 8-bit sensor port                    | Starts the initialization of a sensor at a given new port |
| 31       | 0x1f     | `S_SENS_ACTIVE` | 8-bit sensor no. + 8-bit status (1 or 0)                 | Activates or deactivates a sensor                         |
| 32       | 0x20     | `G_SENS_ACTIVE` | 8-bit sensor no.                                         | Get the activation status of an sensor                    |
| 70       | 0x46     | `S_AUTO_UPDATE` | 8-bit status (1 or 0)                                    | Activates or deactivates the automatic sensor update      |
| 71       | 0x47     | `G_AUTO_UPDATE` | -                                                        | Get the activation stauts of the auto update              |
| 72       | 0x48     | `MAN_UPDATE`    | 8-bit sensor no.                                         | Performs a manual update of the given sensor              |
| 100      | 0x64     | `READ_SENSOR`   | 8-bit sensor no.                                         | Request the sensor reading of the specified sensor        |


## Message Flow

| Requested       | Answer? | MessageType of Answer | Data Bytes of Answer                                                      | Payload-Size [bytes] |
|-----------------|---------|-----------------------|---------------------------------------------------------------------------|----------------------|
| `NONE`          | no      | -                     | -                                                                         | -                    |
| `ACK_FAULT`     | no      | -                     | -                                                                         | -                    |
| `ACK`           | no      | -                     | -                                                                         | -                    |
| `PONG`          | yes     | `ACK`                 | `PONG` + 8-bit random value from request                                  | 1 + 1 = 2            |
| `G_STATUS`      | yes     | `ACK`                 | `G_STATUS` +  8-bit status code                                           | 1 + 1 = 2            |
| `G_COM_ERROR`   | yes     | `ACK`                 | `G_COM_ERROR` + 8-bit communication error code                            | 1 + 1 = 2            |
| `G_SMU_ERROR`   | yes     | `ACK`                 | `G_SMU_ERROR` + 8-bit SMU error code                                      | 1 + 1 = 2            |
| `RESET`         | yes     | `ACK`                 | `RESET`                                                                   | 1                    |
| `FIRMWARE_V`    | yes     | `ACK`                 | `FIRMWARE_V` + 16-bit version                                             | 1 + 1 = 2            |
| `COM_BACK_V`    | yes     | `ACK`                 | `COM_BACK_V` + 16-bit version                                             | 1 + 1 = 2            |
| `INIT_SENSOR`   | yes     | `ACK`                 | `INIT_SENSOR` + success byte (1 or 0) + assigned 8-bit sensor no.         | 1 + 2 = 3            |
| `S_SENS_ACTIVE` | yes     | `ACK`                 | `S_SENS_ACTIVE` + success byte (1 or 0)                                   | 1 + 1 = 2            |
| `G_SENS_ACTIVE` | yes     | `ACK`                 | `G_SENS_ACTIVE` + success byte (1 or 0) + activation status byte (1 or 0) | 1 + 2 = 3            |
| `S_AUTO_UPDATE` | yes     | `ACK`                 | `G_AUTO_UPDATE` + success byte (1 or 0)                                   | 1 + 1 = 2            |
| `G_AUTO_UPDATE` | yes     | `ACK`                 | `S_AUTO_UPDATE` + success byte (1 or 0) + activation status byte (1 or 0) | 1 + 1 = 2            |
| `MAN_UPDATE`    | yes     | `ACK`                 | `MAN_UPDATE` + success byte (1 or 0)                                      | 1 + 1 = 2            |
| `READ_SENSOR`   | yes     | `ACK`                 | `READ_SENSOR` + success byte (1 or 0) + array of sensor readings          | up to 25             |

If the format of a received message is incorrect or the checksum does not match, the following must be answered:

| MessageType of Answer | Data Bytes of Answer                                              | Payload-Size [bytes] |
|-----------------------|-------------------------------------------------------------------|----------------------|
| `ACK_FAULT`           | received message type + communication error information           | up to 25             |


## communication error information

| Error Code        | Error Code [HEX] | Error Description              | Data Bytes                                                     |
|-------------------|------------------|--------------------------------|----------------------------------------------------------------|
| `NO_COM_ERROR`    | 0x00             | no error                       | -                                                              |
| `NO_START_SIGN`   | 0x01             | no start sign was found        | -                                                              |
| `NO_END_SING`     | 0x02             | no end sign was found          | opt. rec. msg. type                                            |
| `INV_PAYL_SIZE`   | 0x03             | the payload-size is invalid    | opt. rec. msg. type + opt. rec. payload-size                   |
| `INV_CHECKSUM`    | 0x04             | the checksum is invalid        | opt. rec. msg. type + opt. rec. checksum + opt. calc. checksum |
| `NOT_ENOUGH_DATA` | 0x05             | not enough data bytes received | opt. rec. msg. type + opt. overall rec. number of bytes        |
| `REC_TIMEOUT`     | 0x06             | timeout during rec of data     | opt. rec. msg. type + opt. overall rec. number of bytes        |
