#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>

#define HEADERSIZE sizeof(header_t)

enum COMMAND{DATA, REQUEST, ACK, FIRST_PACKET, LAST_PACKET, LAST_ACK};

typedef struct{
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
This function extract header info from buffer
**/
void extractHeader(char* buffer, header_t* header);


/**
buffer: buffer to put data
data: data to append to buffer
dataLength: length of data to append to buffer
**/
void appendData(char* buffer, char* data, unsigned int dataLength);


/**
buffer: buffer to calculate checksum
**/
uint16_t calculateChecksum(char*buffer);

/**
 buffer: buffer contains data and header
 **/
char* extractData(char* buffer);

#endif
