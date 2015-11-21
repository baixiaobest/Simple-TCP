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
#include <fcntl.h>
#include <unistd.h>
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
    
    //append data to header, calculate checksum, then send it out
    constructHeader(buffer, header);
    appendData(buffer, fileName, dataLength);
    uint16_t checkSum = calculateChecksum(buffer);
    header.checkSum_m = checkSum;
    constructHeader(buffer, header);
    send(gobackn->socket_m, buffer, (size_t) (strlen(fileName)+HEADERSIZE+1), 0);
    
    //IMPLEMENT: 1. Wait for response from sender.
    //           2. If sender doesn't response, return and report error.
    //           3. If the first packet from sender is received, check command field of packet. if the command field is FIRST_PACKET, set gobackn->seqstart_m to be sequence number + 1. Then ACK the packet. If command field is NOT FIRST_PACKET, resend file request.
    //           4. ACK the sender of last received IN-ORDER uncorrupted packet, remember to put ACK in command field of packet.
    //           5. If packet is not corrupted write the data to file descriptor (Save Data).
    //           6. When receiver receive the packet with command field of LAST_PACKET, write last packet to file descriptor and return.
    
    return 0;
}

int listenForRequest(gobackn_t* gobackn, sockaddr_in* receiverAddr, socklen_t* addrlen){
    char buffer[MAX_PACKET_SIZE];
    header_t header;
    char filename[MAX_FILENAME_SIZE];
    uint32_t nextSeqNumber = gobackn->seqstart;
    
    recvfrom(gobackn->socket_m, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr*) receiverAddr, addrlen);
    extractHeader(buffer, &header);
    uint16_t checksum = calculateChecksum(buffer);
    //drop packet if it is corrupted
    if (checksum != 0) {
        return -1;
    }
    strcpy(filename, &buffer[HEADERSIZE]);
    
    //open the file
    gobackn->fd_m = open(filename, O_RDONLY);
    if (gobackn->fd_m < 0) {
        return -1;
    }
    return 0;
}


int sendRequestedFile(gobackn_t* gobackn, sockaddr_in* receiverAddr){
	//IMPLEMENT: 1. Read data from file descriptor in size of MAX_PACKET_SIZE - HEADER_SIZE.
    //           2. For the first packet, insert FIRST_PACKET in command field.
    //           3. Send the packets upto window size.
    //           4. While sending the packets, also check for ACK packets from receiver
    //           5. A newly received ACK should refresh the timer in gobackn->timer_m
    //           6. If current time - gobackn->timer_m > TIMER_EXPIRE, restart the packet sending process from next packet of last ACK packet.
    //           7. If you read to end of file, construct the last packet with LAST_PACKET set in command field, then send it out. Wait for ACK.
    return 0;
}