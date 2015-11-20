//
//  Sender.cpp
//  TCP
//
//  Created by Baixiao Huang on 11/19/15.
//  Copyright (c) 2015 Baixiao Huang. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>

#include "Header.h"
#include "GobackN.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    int socketfd, portNumber;
    struct sockaddr_in myAddress, receiverAddr;
    char* receiverIP;
    int receiverPort;
    char buffer[MAX_PACKET_SIZE];
    char *dataBuffer;
    header_t header;
    if (argc < 2) {
        cout << "Expect port number";
        return 1;
    }
    portNumber = atoi(argv[1]);
    //define receiver address
    bzero((char*) &myAddress, sizeof(myAddress));
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);  //UDP
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = INADDR_ANY; //accept any incoming connection
    myAddress.sin_port = htons(portNumber); //bind to any port
    
    if (bind(socketfd, (struct sockaddr*) &myAddress, sizeof(myAddress)) < 0) {
        cout << "Failed to bind to port" << endl;
    }
    socklen_t addrlen;
    //first get the header
    recvfrom(socketfd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &receiverAddr, &addrlen);
    receiverIP = inet_ntoa(receiverAddr.sin_addr);
    receiverPort = ntohs(receiverAddr.sin_port);
    
    cout << "Receiver IP: " << receiverIP << "Receiver Port: " << receiverPort << endl;
    extractHeader(buffer, &header);
    uint16_t checksum = calculateChecksum(buffer);
    cout << "Checksum is: " << checksum << endl;
    cout << "Requesting file: ";
    fwrite(&buffer[HEADERSIZE], header.dataLength_m, 1, stdout);
    
    return 0;
}
