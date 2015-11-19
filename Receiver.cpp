#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>

#include "Header.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    int socketfd, senderPort;
    struct sockaddr_in senderAddress;
    struct hostent* sender;
    char* fileName;
    header_t header;
    char buffer[HEADERSIZE];
    if (argc < 4) {
        cout << "no enough arguments." << endl;
        return 1;
    }
    fileName = argv[3];
    
    //define sender address
    sender = gethostbyname(argv[1]);
    senderPort = atoi(argv[2]);
    if (sender==NULL) {
        cout << "no such host" << endl;
        return 1;
    }
    bzero((char*) &senderAddress, sizeof(senderAddress));
    senderAddress.sin_family = AF_INET;
    bcopy((char *)sender->h_addr,
          (char *)&senderAddress.sin_addr.s_addr,
          sender->h_length);
    senderAddress.sin_port = htons(senderPort);
    
    //define receiver address
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);  //UDP
    
    //define header info
    header.checkSum_m = uint16_t(0);
    header.sequenceNumber_m = uint32_t(1);
    header.ACKNumber_m = uint32_t(0);
    header.dataLength_m = uint16_t(0);
    header.command_m = uint16_t(1);
    //construct pseudo header with checksum = 0
    constructHeader(buffer, header);
    uint16_t checkSum = calculateChecksum(buffer);
    header.checkSum_m = checkSum;
    constructHeader(buffer, header);
    
    if(sendto(socketfd, (void*) buffer, (size_t)20, 0, (struct sockaddr *) &senderAddress, sizeof(senderAddress)) < 0){
        cout << "fail to send data" << endl;
    }
    
	return 0;
}
