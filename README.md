# Simple TCP implementation with UDP

This simple TCP protocol is implemented on top of UDP, and uses **Go Back N** protocol to acheive reliable data transfer.
There is a receive that request the file and a sender that send the file.


## Packet Header Design

|  16 Bit Field   |   16 Bit Field   |
|-----------------|------------------|
|Source IP Address|Source IP Address |
|   source Port   |     checksum     |
| Sequence Number | Sequence Number  |
|    ACK Number   |     ACK Number   |
|   Data Length   |     Command      |
|...............DATA.................|

This is 32bit in width.

There are 3 command types:
* 0: Packet is used for data transfer
* 1: Packet is a request for file
* 2: Packet is ACK packet

In case of ACK packet, data field contains no packet, data length is thus zero.

### Maximum data field size
The maximum size of packet is 1 Kb, thus the maximum size of data a packet can carry over UDP is 1004 bytes.


## Checksum Calculation
On the sender side, header and data **Except** checksum field are added together as 16bit addition. After EACH addition, if there is a overflow, the overflow part (16 bit field and higher) are added back to 0 to 16 bit field. After all addition, 1's complement of the result is performed, and you get the checksum.

On the receiver side, the checksum is done to header (including checksum field) and data. If the result is zeros, then there is no data corrupt, otherwise, corruption occurs

In case that packet size is not size of multiple of 16 bits, we append padding zeros to data field during calculation.
