#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>

enum checkSumMode {CHEKSUM_INC, CHEKSUM_NOTINC};
typedef struct{
	int socketFd_m;
	char* IPAddress_m;  //Receiver IP address
	uint16_t portNumber_m;   //Receiver port number
	uint16_t checkSum_m;
	uint32_t sequenceNumber_m;
	uint32_t ACKNumber_m;
	uint16_t dataLength_m;
	uint16_t command_m;
} header_t;

/**
buffer: buffer to put header
header_t: header information
NOTICE: When header is feed into buffer, dataLength and checkSum are zeros.
**/
void constructHeader(char* buffer, header_t header);


/**
buffer: buffer to put data
data: data to append to buffer
dataLength: length of data to append to buffer
return: return the actual length of data appended to buffer. Because dataLength might not multiple of 16 bits, extra padding zeros may be added to the back of buffer. This function will also change the dataLenth field of buffer.
**/
unsigned int appendData(char* buffer, char* data, unsigned int dataLength);


/**
buffer: buffer to calculate checksum
checkSumMode: whether to calculate checksum with checksum field included.
**/
uint16_t calculateChecksum(char*buffer, checkSumMode mode);

#endif
