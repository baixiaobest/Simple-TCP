//
//  GobackN.cpp
//  TCP
//
//  Created by Baixiao Huang on 11/19/15.
//  Copyright (c) 2015 Baixiao Huang. All rights reserved.
//
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <string.h>
#include "GobackN.h"

int requestFile(gobackn_t* gobackn, char* fileName){
    //initialize request file header.
    header_t header;
    header.checkSum_m = 0;
    header.sequenceNumber_m = 0; //Receiver doesn't need sequence number
    header.ACKNumber_m = 0; //first request is not ACKing anything
    int dataLength = (int) (strlen(fileName)+1); //including terminating null
    header.dataLength_m = (uint16_t) dataLength;
    header.command_m = (uint16_t) REQUEST;
    char buffer[strlen(fileName)+HEADERSIZE];
    
    constructHeader(buffer, header);
    appendData(buffer, fileName, dataLength);
    uint16_t checkSum = calculateChecksum(buffer);
    header.checkSum_m = checkSum;
    constructHeader(buffer, header);
    send(gobackn->socket_m, buffer, (size_t) (strlen(fileName)+HEADERSIZE+1), 0);
    
    return 0;
}

int listenForRequest(gobackn_t* gobackn){
    
    return 0;
}