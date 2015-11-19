#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#include "Header.h"


void constructHeader(char* buffer, header_t header){
	//fill up checkSum field
	buffer[0] = (char) (header.checkSum_m >> 8) & 0xFF;
	buffer[1] = (char) header.checkSum_m & 0xFF;
	//fill up sequence number field
	buffer[2] = (char) ((header.sequenceNumber_m >> 24) & 0xFF);
	buffer[3] = (char) ((header.sequenceNumber_m >> 16) & 0xFF);
	buffer[4]= (char) ((header.sequenceNumber_m >> 8)  & 0xFF);
	buffer[5]= (char) ( header.sequenceNumber_m        & 0xFF);
	//fill up ACK number field
	buffer[6] = (char) (header.ACKNumber_m >> 24 & 0xFF);
	buffer[7] = (char) (header.ACKNumber_m >> 16 & 0xFF);
	buffer[8] = (char) (header.ACKNumber_m >> 8  & 0xFF);
	buffer[9] = (char) (header.ACKNumber_m       & 0xFF);
	//fill up dataLength field
	buffer[10] = (char) (header.dataLength_m >> 8 & 0xFF);
	buffer[11] = (char) (header.dataLength_m      & 0xFF);
	//fill up command field
	buffer[12] = (char) (header.command_m >> 8) & 0xFF;
	buffer[13] = (char) (header.command_m       & 0xFF);
	return;
}

void extractHeader(char* buffer, header_t* header){
    header->checkSum_m = ( ((uint16_t)buffer[0]) << 8 ) | ( (uint16_t) buffer[1] );
    header->sequenceNumber_m = (( (uint32_t) buffer[2] ) << 24) | ( ((uint32_t) buffer[3]) << 16 ) | (((uint32_t) buffer[4]) << 8) | ((uint32_t) buffer[5]);
    header->ACKNumber_m = (( (uint32_t) buffer[6] ) << 24) | ( ((uint32_t) buffer[7]) << 16 ) | (((uint32_t) buffer[8]) << 8) | ((uint32_t) buffer[9]);
    header->dataLength_m = ( ((uint16_t)buffer[10]) << 8 ) | ( (uint16_t) buffer[11] );
    header->command_m = ( ((uint16_t)buffer[12]) << 8 ) | ( (uint16_t) buffer[13] );
}

void appendData(char* buffer, char* data, unsigned int dataLength){
    int offset = 20;
    for (int i=0; i<dataLength; i++) {
        buffer[offset+i] = data[i];
    }
    return;
}

uint16_t calculateChecksum(char*buffer){
    uint16_t dataLength = ((uint16_t)buffer[10]) << 8 | ((uint16_t)buffer[11] & 0xFF);
    uint32_t checksum = 0;
    int iteration = (HEADERSIZE+dataLength)/2;
    for (int i=0; i<iteration; i++) {
        uint16_t hi = ((uint16_t) buffer[i*2] ) << 8;
        uint16_t lo = ((uint16_t)buffer[i*2+1]) & 0xFF;
        checksum = checksum + (uint32_t)( hi | lo);
        checksum = (checksum & 0x0000FFFF) + ((checksum & 0xFFFF0000) >> 16);
    }
    //pad with zeros
    if ((HEADERSIZE+dataLength)%2 == 1) {
        checksum = checksum + (uint32_t)(((uint16_t)buffer[iteration*2]) << 8);
        checksum = (checksum & 0x0000FFFF) + ((checksum & 0xFFFF0000) >> 16);
    }
    //one's complement
    checksum = ~checksum;
    return (uint16_t) (checksum & 0x0000FFFF);
}
