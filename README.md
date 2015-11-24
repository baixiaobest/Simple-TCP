# Simple TCP implementation with UDP

This simple TCP protocol is implemented on top of UDP, and uses **Go Back N** protocol to acheive reliable data transfer.
There is a receive that request the file and a sender that send the file.


## Packet Header Design

|  16 Bit Field   |   16 Bit Field   |
|-----------------|------------------|
|     checksum    |                  |
| Sequence Number | Sequence Number  |
|    ACK Number   |     ACK Number   |
|   Data Length   |     Command      |
|...............DATA.................|

This is 32bit in width.

There are 4 command types:
* 0: Packet is used for data transfer
* 1: Packet is a request for file
* 2: Packet is ACK packet
* 3: First data packet
* 4: Last data packet
* 5: ACK for the last packet

In case of ACK packet, data field contains no packet, data length is thus zero.
Sender will put command 3 into first packet sent out and command 4 into last packet sent out, so that receiver knows where to start and where to end.
Receiver acknowlege received last in order packet sequence number.

### Maximum data field size
The maximum size of packet is 1 Kb, the header size is 14 bytes, thus the maximum size of data a packet can carry over UDP is 1010 bytes.


## Checksum Calculation
On the sender side, header and data **Except** checksum field are added together as 16bit addition. After EACH addition, if there is a overflow, the overflow part (16 bit field and higher) are added back to 0 to 16 bit field. After all addition, 1's complement of the result is performed, and you get the checksum.

On the receiver side, the checksum is done to header (including checksum field) and data. If the result is zeros, then there is no data corruption, otherwise, corruption occurs.

In case that packet size is not size of multiple of 16 bits, we append padding zeros to data field during calculation.


## Go Back N

### Receiver
Receiver will maintain a fixed window size. The start of window is seqstart number. This number is initiated when the sender sends the first packet of data with command field set to 3. Receiver will then grab the sequence number from the packet and set seqstart to that sequence number.

### Sender
Sender will also maintain a fixed window size. This window size as the same size of Receiver's window size. The start of window is seqstart number. This number is randomly picked from a pool of available sequence number. The seqstart will shift forward (increment) only when the packet with sequence number of seqstart is ACKed. When sender is sending the last packet, it marks the last data packet with 4 in command field.


## How to compile
type 
	$:make 
in terminal, two executables will be built, receiver and sender.
To run receiver:
	$:./receiver <senderIP> <senderPort> <filename>
To run sender:
	$:./sender <port to bind>

**NOTICE**: You need to run sender before running receiver.