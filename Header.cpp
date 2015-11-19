#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#include "Header.h"


void constructHeader(char* buffer, header_t header){
	//fill up source IP field
	sockaddr_in myAddr;
	inet_aton(header.IPAddress_m, &myAddr.sin_addr);
	buffer[0] = (char) ((myAddr.sin_addr.s_addr >> 24) & 0xFF);
	buffer[1] = (char) ((myAddr.sin_addr.s_addr >> 16) & 0xFF);
	buffer[2] = (char) ((myAddr.sin_addr.s_addr >> 8)  & 0xFF);
	buffer[3] = (char) (myAddr.sin_addr.s_addr       & 0xFF);
	//fill up source port field
	buffer[4] = (char) (header.portNumber_m >> 8) & 0xFF;
	buffer[5] = (char) header.portNumber_m & 0xFF;
	//fill up checkSum field
	buffer[6] = (char) (header.checkSum_m >> 8) & 0xFF;
	buffer[7] = (char) header.checkSum_m & 0xFF;
	//fill up sequence number field
	buffer[8] = (char) ((header.sequenceNumber_m >> 24) & 0xFF);
	buffer[9] = (char) ((header.sequenceNumber_m >> 16) & 0xFF);
	buffer[10]= (char) ((header.sequenceNumber_m >> 8)  & 0xFF);
	buffer[11]= (char) ( header.sequenceNumber_m        & 0xFF);
	//fill up ACK number field
	buffer[12] = (char) (header.ACKNumber_m >> 24 & 0xFF);
	buffer[13] = (char) (header.ACKNumber_m >> 16 & 0xFF);
	buffer[14] = (char) (header.ACKNumber_m >> 8  & 0xFF);
	buffer[15] = (char) (header.ACKNumber_m       & 0xFF);
	//fill up dataLength field
	buffer[16] = (char) (header.dataLength_m >> 8 & 0xFF);
	buffer[17] = (char) (header.dataLength_m      & 0xFF);
	//fill up command field
	buffer[18] = (char) (header.command_m >> 8) & 0xFF;
	buffer[19] = (char) (header.command_m       & 0xFF);
	return;
}
