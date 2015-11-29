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
#include <unistd.h>


#include "Header.h"
#include "GobackN.h"
#include <iostream>
#include <signal.h>

using namespace std;

gobackn_t gobackn;
socklen_t addrlen;
struct sockaddr_in myAddress;

void sighandler(int);
void timeout_handler(int);

int main(int argc, char* argv[]){
    signal(SIGINT, sighandler);
    signal(SIGALRM, timeout_handler);
    itimerval timerval;
    timeval timeout_val;
    timeout_val.tv_sec = TIMEOUT_INTERVAL/1000;
    timeout_val.tv_usec = (TIMEOUT_INTERVAL*1000)%1000000;
    timerval.it_value = timeout_val;
    timerval.it_interval = timeout_val;
    gobackn.timer = &timerval;
    
    int socketfd, portNumber, windowSize;

    char *dataBuffer;
    header_t header;
    if (argc < 3) {
        cout << "No enough inputs";
        return 1;
    }
    portNumber = atoi(argv[1]);
    windowSize = atoi(argv[2]);
    //define receiver address
    bzero((char*) &myAddress, sizeof(myAddress));
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);  //UDP
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = INADDR_ANY; //accept any incoming connection
    myAddress.sin_port = htons(portNumber); //bind to any port
    
    if (bind(socketfd, (struct sockaddr*) &myAddress, sizeof(myAddress)) < 0) {
        cout << "Failed to bind to port" << endl;
        return 1;
    }
    
    gobackn.socket_m = socketfd;
    gobackn.seqstart_m = 0;
    gobackn.seqend_m = 0 + windowSize * MAX_DATA_SIZE;
    gobackn.initial = true;
    sockaddr_in receiverAddr;
    while (true) {
        cout << "Info: Waiting for request" << endl;
        if(listenForRequest(&gobackn, receiverAddr, addrlen) == -1){
            cout << "Error: File not found, an empty file will be sent" << endl;
        }
    
        sendRequestedFile(&gobackn, receiverAddr, addrlen);
        
       
        //reset gobackn for next request
        gobackn.fd_m = NULL;
        gobackn.seqstart_m = 0;
        gobackn.seqend_m = 0 + windowSize * MAX_DATA_SIZE;
    }
    
    return 0;
}

void sighandler(int signum) {
    if(signum == SIGINT){
        cout << "INFO: Interuption detected, clean up the program" << endl;
        fclose(gobackn.fd_m);
        close(gobackn.socket_m);
        _exit(1);
    }
}

void timeout_handler(int signum) {
    if(signum == SIGALRM){
        //dummy value
        bool lastPacketSent;
        sendData(gobackn.seqstart_m, gobackn.seqend_m, &gobackn, gobackn->initial, myAddress, addrlen, lastPacketSent);
        cout << "INFO: Timeout! Sender resends data from " << gobackn.seqstart_m << " to " << gobackn.seqend_m <<endl;
    }
}
