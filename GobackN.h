//
//  GobackN.h
//  TCP
//
//  Created by Baixiao Huang on 11/19/15.
//  Copyright (c) 2015 Baixiao Huang. All rights reserved.
//

#ifndef GobackN_H
#define GobackN_H

#include <stdio.h>
#include <time.h>
#include "Header.h"


#define MAX_PACKET_SIZE 1024  //size in bytes for the whole packet
#define MAX_DATA_SIZE (MAX_PACKET_SIZE - HEADERSIZE)    //size in bytes for the data part

typedef struct{
     /*requested file descriptor. For receiver, it is the file on disk to save to.
      For sender, it is the file to send to socket_m*/
    FILE* fd_m;
    //For receiver, this is socket to sender. For sender, this is socket from reciever
    int socket_m;
    //first sequence number in the window (inclusive)
    uint32_t seqstart_m;
    //last sequence number in the window (exclusive)
    uint32_t seqend_m;
    //a timer that is refreshed when new ACK is received. This is for sender
    clock_t timer_m;
    int dataLossProb;
    int dataCorruptProb;
} gobackn_t;

/*
This function sends request for a certain file to sender, and get all the file data to file descriptor of gobackn. This function should be called by receiver.
@ gobackn: struct to keep info about window size and sequence number. You need specify file descriptor and sender socket. seqstart_m of gobackn is defined by sender.
@ fileName: the file you are requesting.
@ Return: it return when finished getting all files.
 returns -1 when it fails to get file. >= 0 on success
*/
int requestFile(gobackn_t* gobackn, char* fileName);

/*
This function listen for connection, when a request for file is received, function open up file descriptor for file and returns. This is a blocking function.
@ gobackn: It keeps info about connection and window size. seqstart_m and seqend_m should be specified before calling this function
@ receiverAddr receiver's address
@ addrlen address length
@ return: -1 when requested file not found. >=0 on success
*/

int listenForRequest(gobackn_t* gobackn, sockaddr_in& receiverAddr, socklen_t& addrlen);

/*
This function sends requested file to receiver.
*/

int sendRequestedFile(gobackn_t* gobackn,sockaddr_in receiverAddr, socklen_t addrlen);


ssize_t Send(gobackn_t* gobackn, void *buf, size_t len, int flags);

ssize_t SendTo(gobackn_t* gobackn, void *buff, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

/*
This function will modify the checksum field of buff and return 1 if data is corrupted.
 It returns -1 if data is lost. Return 0 if nothing happens.
 */
int dataLossCorruptionSim(void *buff, int lossProb, int corruptProb);


#endif
