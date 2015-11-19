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

void appendData(char* buffer, char* data, unsigned int dataLength){
    int offset = 20;
    for (int i=0; i<dataLength; i++) {
        buffer[offset+i] = data[i];
    }
    return;
}
