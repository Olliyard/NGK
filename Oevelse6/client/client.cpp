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

using namespace std;

/*
void receiveFile(string fileName, int socketfd){
    int n;
    FILE *fp;
    char *filename = "recv.txt";
    char buffer[SIZE];
    
    fp = fopen(filename, "w");
    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0){
        break;
        return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
}
*/

int main(int argc, char *argv[])
{
    int buffersize = 0, recv_size = 0, size = 0, r_size, w_size;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1000];

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
    
    bzero(buffer,256);
    if((n = read(sockfd,buffer,255)) <0){
        error("ERROR reading from socket");
    }
    cout << buffer << endl;
    
    cout << "Please enter the message: ";
    


    bzero(buffer,256);
    fgets(buffer,255,stdin);
    writeTextTCP(sockfd, buffer);
    /*
    if((n = write(sockfd,buffer,strlen(buffer))) < 0){
        error("ERROR writing to socket");
    }
    */
    //receive_file(sockfd);
    //cout << "File received" << endl;

    close(sockfd);
    return 0;
}