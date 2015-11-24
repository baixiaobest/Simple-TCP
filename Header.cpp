#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#include "Header.h"


void constructHeader(char* buffer, header_t header){
    memcpy(buffer, &header, HEADERSIZE);
    return;
}

void extractHeader(char* buffer, header_t* header){
    memcpy(header, buffer, HEADERSIZE);
    return;
}

void appendData(char* buffer, char* data, unsigned int dataLength){
    int offset = HEADERSIZE;
    for (int i=0; i<dataLength; i++) {
        buffer[offset+i] = data[i];
    }
    return;
}

uint16_t calculateChecksum(char*buffer){
    header_t header;
    memcpy(&header, buffer, HEADERSIZE);
    int originalCheckSum = header.checksum;
    header.checksum = 0;
    memcpy(buffer, header, HEADERSIZE);

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
    header.checksum = originalCheckSum;
    memcpy(buffer, header, HEADERSIZE);
    return (uint16_t) (checksum & 0x0000FFFF);
}

char* extractData(char* buffer){
    return buffer + HEADERSIZE;
}
