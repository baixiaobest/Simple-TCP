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
#include <unistd.h>
#include <signal.h>

using namespace std;

gobackn_t gobackn_g;
void sighandler(int);

int main(int argc, char* argv[]){
    signal(SIGINT, sighandler);

    int socketfd, senderPort;
    struct sockaddr_in senderAddress;
    struct hostent* sender;
    char* fileName;
    header_t header;
    char buffer[HEADERSIZE];
    if (argc < 6) {
        cout << "no enough arguments." << endl;
        return 1;
    }
    fileName = argv[3];
    
    //define sender address
    sender = gethostbyname(argv[1]);
    senderPort = atoi(argv[2]);
    gobackn_g.dataLossProb = atoi(argv[4]);
    gobackn_g.dataCorruptProb = atoi(argv[5]);
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
    
    //define receiver socket
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);  //UDP
    
    if(connect(socketfd, (struct sockaddr *) &senderAddress, sizeof(senderAddress)) < 0){
        cout << "connection fails" << endl;
    }
    gobackn_g.socket_m = socketfd;
    gobackn_g.seqend_m = 0;
    requestFile(&gobackn_g, fileName);

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
