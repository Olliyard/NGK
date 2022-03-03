#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "iknlib.h"
#define SIZE 1000
using namespace std;

int main(int argc, char *argv[])
{
    int buffersize = 0, recv_size = 0, size = 0, r_size, w_size;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[SIZE];

    if (argc < 3) {
       error("ERROR no hostname/port");
    }

    portno = atoi(argv[2]);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("ERROR no such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR connecting");
    }
    
    //reads text over TCP. Message is stored in 'file'
    if(readTextTCP(buffer, SIZE, sockfd) <0){
        error("ERROR on reading");
    }
    cout << buffer << endl;
    
    cout << "Please enter the message: ";
    
    //write to server
    bzero(buffer,SIZE);
    fgets(buffer,SIZE,stdin);
    writeTextTCP(sockfd, buffer);

    close(sockfd);
    return 0;
}