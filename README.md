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

| Dec-Code | Hex-Code | MessageType   | Data Bytes                                               | Description                                               |
|----------|----------|---------------|----------------------------------------------------------|-----------------------------------------------------------|
| 0        | 0x00     | `NONE`        | -                                                        | Empty message                                             |
| 1        | 0x01     | `ACK_FAULT`   | 8-bit message-type of last rec message + error code      | Indicates a coomunication error                           |
| 2        | 0x02     | `ACK`         | 8-bit message-type that is been acknowledged + payload   | ACK + payload send back (can be up to 24 bytes)           | 
| 3        | 0x03     | `PONG`        | 8-bit random value                                       | Ping Request                                              |
| 4        | 0x04     | `GET_STATUS`  | 8-bit status code                                        | Get the current system stauts code from the SMU           |
| 5        | 0x05     | `GET_ERROR`   | 8-bit error code                                         | Get the current system error code from the SMU            |
| 6        | 0x06     | `RESET`       | -                                                        | Perform a soft reset of the SMU                           |
| 30       | 0x1e     | `INIT_SENSOR` | 8-bit sensor no. + 8-bit sensor-type + 8-bit sensor port | Starts the initialization of a sensor at a given new port |
| 31       | 0x1f     | `SET_ACTIVE`  | 8-bit sensor no. + 8-bit status (1 or 0)                 | Activates or deactivates a sensor                         |
| 70       | 0x46     | `AUTO_UPDATE` | 8-bit status (1 or 0)                                    | Activates or deactivates the automatic sensor update      |
| 71       | 0x47     | `MAN_UPDATE`  | 8-bit snesor no.                                         | Performs a manual update of the given sensor              |
