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
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

/*
 This function send the content of the file indicated by begin and end to the socket indicated in gobackn_t.
 It will truncate the data into packets.
 The file pointer will stop at the end position.
 @ begin: begin position of the content to be sent (inclusive)
 @ end: end position of the content to be sent (exclusive)
 @ gobackn: struct to keep info about window size and sequence number. You need specify file descriptor and sender socket. seqstart_m of gobackn is defined by sender.
 @initial: indicate whether it is the first time that this function is called
 @lastPacketSent: lastPacketSent will be set to be true after the last packet has been sent
 return 0 when succeed, return -1 when an error occurs.
 */
int sendData(uint32_t begin, uint32_t end, gobackn_t* gobackn, bool initial,sockaddr_in receiverAddr, socklen_t addrlen, bool& lastPacketSent);

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
    cout << "Request for file " << fileName << endl;
    send(gobackn->socket_m, buffer, (size_t) (strlen(fileName)+HEADERSIZE+1), 0);
    
    //create a new file to save data into it
    gobackn -> fd_m = fopen(fileName, "w");
    if(gobackn -> fd_m == NULL){
        cout << "Error: cannot create file to write" << endl;
        close(gobackn->socket_m);
        return -1;
    }
    
    char dataBuffer[MAX_PACKET_SIZE];
    memset(dataBuffer, 0, MAX_PACKET_SIZE);

    while (true) {
        struct sockaddr_in receiverAddr;
        socklen_t addrlen;
        //TODO:remove the line for debugging
        cout <<"Waiting for response" << endl;
        if(recvfrom(gobackn->socket_m, dataBuffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &receiverAddr, &addrlen) == -1){
            cout << "Error: when receiving data from sender, recvfrom return -1" << endl;
            close(gobackn->socket_m);
            fclose(gobackn->fd_m);
            return -1;
        }
        extractHeader(dataBuffer, &header);
        //we may receive obsolete data packets from the sender
        if(header.command_m != (uint16_t) DATA && header.command_m != (uint16_t) LAST_PACKET)
            continue;
        
        cout << "Info: Receiver receives a packet from sender with Sequence: " << header.sequenceNumber_m << " lenghth: " << header.dataLength_m << endl;
        
        char* dataReceived = extractData(dataBuffer);
        header_t ackHeader;
        //if the packet is the next expected packet
        if(gobackn->seqend_m == header.sequenceNumber_m){
            fwrite(dataReceived,sizeof(char) ,header.dataLength_m, gobackn ->fd_m);
            ackHeader.ACKNumber_m = header.sequenceNumber_m + header.dataLength_m;
            gobackn->seqend_m += header.dataLength_m;
        }
        //if the packet is not the expected packet
        else{
            ackHeader.ACKNumber_m = gobackn->seqend_m;
        }
        
        //send ACK back to sender
        ackHeader.sequenceNumber_m = 0;
        ackHeader.checkSum_m = 0;
        ackHeader.dataLength_m = 0;
        uint16_t checkSum = calculateChecksum(dataBuffer);
        ackHeader.checkSum_m = checkSum;
        if(header.command_m == (uint16_t)LAST_PACKET){
            ackHeader.command_m = (uint16_t) LAST_ACK;
        }
        else{
            ackHeader.command_m = (uint16_t) ACK;
        }
        constructHeader(dataBuffer, ackHeader);
        cout << "Info: Receiver sends an ACK with ACK number: " << ackHeader.ACKNumber_m << endl;
        send(gobackn->socket_m, dataBuffer, HEADERSIZE, 0);
        
        //if the packet processed was the last packet, break out of the loop
        if(header.command_m == (uint16_t) LAST_PACKET){
            break;
        }
        memset(dataBuffer, 0, MAX_PACKET_SIZE);
    }
    
    fclose(gobackn->fd_m);
    close(gobackn->socket_m);
    return 0;
}

int listenForRequest(gobackn_t* gobackn, sockaddr_in& receiverAddr, socklen_t& addrlen){
    char* receiverIP;
    int receiverPort;
    char buffer[MAX_PACKET_SIZE];
    int socketfd = gobackn -> socket_m;
    header_t header;
    
    //we may receive obsolete ACKs from receiver
    while (true) {
        //first get the header
        recvfrom(socketfd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &receiverAddr, &addrlen);
        receiverIP = inet_ntoa(receiverAddr.sin_addr);
        receiverPort = ntohs(receiverAddr.sin_port);
        
        extractHeader(buffer, &header);
        uint16_t checksum = calculateChecksum(buffer);
        if(header.command_m == (uint16_t) REQUEST){
            cout << "Info: request for file transfer received" << endl;
            cout << "Receiver IP: " << receiverIP << "Receiver Port: " << receiverPort << endl;
            cout << "Checksum is: " << checksum << endl;
            cout << "Requesting file: " << buffer + HEADERSIZE << endl;
            
            gobackn -> fd_m = fopen(extractData(buffer),"r");
            if(gobackn -> fd_m == NULL){
                gobackn -> fd_m = tmpfile();
                return -1;
            }
            else
                return 0;
        }
    }
    
    //should never reach here
    return 0;
}

int sendRequestedFile(gobackn_t* gobackn,sockaddr_in receiverAddr, socklen_t addrlen){
    header_t header;
    char dataBuffer[MAX_PACKET_SIZE];
    bool lastPacketSent = false;

    //send all of the content in the window
    if(sendData(gobackn->seqstart_m, gobackn->seqend_m, gobackn, true, receiverAddr, addrlen, lastPacketSent) == -1){
        cout << "Error: fail to send the data" << endl;
        return -1;
    }
    while (true) {
        struct sockaddr_in receiverAddr;
        socklen_t addrlen;
        if(recvfrom(gobackn->socket_m, dataBuffer, MAX_PACKET_SIZE, 0, (struct sockaddr *) &receiverAddr, &addrlen) == -1){
            cout << "Error: when receiving ACK from receiver, recfrom return -1" << endl;
            close(gobackn->socket_m);
            fclose(gobackn->fd_m);
            return -1;
        }
        extractHeader(dataBuffer, &header);
        if(header.command_m == (uint16_t) LAST_ACK){
            cout << "Info: ACK for last packet received" << endl;
            break;
        }
        else if(header.command_m != (uint16_t) ACK)
            continue;
        cout << "Info: Receive ACK with ACK number " << header.ACKNumber_m << endl;
        
        //ignore ACK which is out of the bound
        if(header.ACKNumber_m < gobackn -> seqstart_m || header.ACKNumber_m >= gobackn->seqend_m)
            continue;
        
        uint32_t newBegin = header.ACKNumber_m;
        uint32_t newEnd = gobackn-> seqend_m;
        //after the last packet has been sent, we don't have to send more packets.
        //we only need to update the (start of) window
        if(!lastPacketSent){
            newBegin = header.ACKNumber_m;
            newEnd =(header.ACKNumber_m - gobackn -> seqstart_m) + gobackn-> seqend_m;
            cout << "new end is " << newEnd << endl;
        
            if(sendData(gobackn -> seqend_m, newEnd, gobackn,false, receiverAddr, addrlen, lastPacketSent) == -1){
                cout << "Error: fail to send the data" << endl;
                return -1;
            }
        }
        gobackn -> seqstart_m = newBegin;
        gobackn -> seqend_m = newEnd;
    }
    fclose(gobackn -> fd_m);
    return 0;
}

int sendData(uint32_t begin, uint32_t end, gobackn_t* gobackn, bool initial,sockaddr_in receiverAddr, socklen_t addrlen, bool& lastPacketSent){
    //the position of file should always be at the end position of gobackn window (seqend_m)
    //after the initial call
    //so we can set the offset with this knowledge
    if(!initial){
        uint32_t offset = begin - gobackn -> seqend_m;
        fseek ( gobackn -> fd_m , offset, SEEK_CUR);
    }
    while (begin < end) {
        header_t header;
        header.checkSum_m = 0;
        header.sequenceNumber_m = begin;
        header.ACKNumber_m = 0; //Sender does not need ack
        
        char buffer[MAX_PACKET_SIZE];
        char data[MAX_DATA_SIZE];
        size_t sizeReaded = fread(data, sizeof(char), MAX_DATA_SIZE, gobackn -> fd_m);
        if(sizeReaded != MAX_DATA_SIZE){
            if (ferror(gobackn -> fd_m)) {
                cout << "Error: cannot read file "<<endl;
                return -1;
            }
            else if(feof(gobackn -> fd_m)){
                cout << "Info: Last packet to be read" << endl;
                header.dataLength_m = sizeReaded;
                header.command_m = (uint16_t) LAST_PACKET;
                lastPacketSent = true;
            }
        }
        else{
            header.dataLength_m = MAX_DATA_SIZE;
            header.command_m = (uint16_t) DATA;
        }
        constructHeader(buffer, header);
        appendData(buffer, data, header.dataLength_m);
        uint16_t checkSum = calculateChecksum(buffer);
        header.checkSum_m = checkSum;
        constructHeader(buffer, header);
        
        cout << "Info: Sender sends packet with sequence number :" << header.sequenceNumber_m << " data length: " << header.dataLength_m << endl;
        if(sendto(gobackn->socket_m, buffer, HEADERSIZE + header.dataLength_m, 0, (struct sockaddr *) &receiverAddr, addrlen) < 0){
            cout << "Error: fail to send data" << endl;
            return -1;
        }
        begin += header.dataLength_m;
        if(header.command_m == (uint16_t) LAST_PACKET)
            break;
    }
    return 0;
}
