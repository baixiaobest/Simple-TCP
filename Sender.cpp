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

#define TIMEOUT_INTERVAL 500
using namespace std;

gobackn_t gobackn_g;

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
    gobackn_g.timer = &timerval;
  
    int socketfd, portNumber, windowSize;
    struct sockaddr_in myAddress;

    char *dataBuffer;
    header_t header;
    if (argc < 5) {
        cout << "No enough inputs";
        return 1;
    }
    portNumber = atoi(argv[1]);
    windowSize = atoi(argv[2]);
    gobackn_g.dataLossProb = atoi(argv[3]);
    gobackn_g.dataCorruptProb = atoi(argv[4]);
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
    
    gobackn_g.socket_m = socketfd;
    gobackn_g.seqstart_m = 0;
    gobackn_g.seqend_m = 0 + windowSize * MAX_DATA_SIZE;
    sockaddr_in receiverAddr;
    socklen_t addrlen;
    while (true) {
        cout << "Info: Waiting for request" << endl;
        if(listenForRequest(&gobackn_g, receiverAddr, addrlen) == -1){
            cout << "Error: File not found, an empty file will be sent" << endl;
        }
        else{
          gobackn_g.receiverAddr = receiverAddr;
          gobackn_g.addrlen = addrlen;
        }
    
        sendRequestedFile(&gobackn_g, receiverAddr, addrlen);
        
        //stop the timer
        timeval zero_time;
        zero_time.tv_sec = 0;
        zero_time.tv_usec = 0;
        gobackn_g.timer -> it_value = zero_time;
        setitimer(ITIMER_REAL, gobackn_g.timer, NULL);
      
        //reset gobackn for next request
        gobackn_g.fd_m = NULL;
        gobackn_g.seqstart_m = 0;
        gobackn_g.seqend_m = 0 + windowSize * MAX_DATA_SIZE;
    }
    
    return 0;
}

void sighandler(int signum) {
    if(signum == SIGINT){
        cout << "Interuption detected, clean up the program" << endl;
        fclose(gobackn_g.fd_m);
        close(gobackn_g.socket_m);
        _exit(1);
    }
}

void timeout_handler(int signum) {
    if(signum == SIGALRM){
        //dummy value
        bool lastPacketSent;
        cout << "INFO: Timeout! Sender resends data from " << gobackn_g.seqstart_m << " to " << gobackn_g.seqend_m <<endl;
      if(sendData(gobackn_g.seqstart_m, gobackn_g.seqend_m, &gobackn_g, false, gobackn_g.receiverAddr, gobackn_g.addrlen, lastPacketSent) < 0){
        cout << "Error: Cannot resend data!" << endl;
      }
    }
}
