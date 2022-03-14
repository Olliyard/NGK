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

void receiveFile(string fileName, int sockfd)
{

}

int main(int argc, char *argv[])
{
    int buffersize = 0, recv_size = 0, size = 0, r_size, w_size;
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256], fbuf[SIZE];

    if (argc < 3) {
       error("ERROR no hostname/filename");
    }

    portno = 9000;
    //portno = atoi(argv[2]);


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
    
    //reads text over TCP. Message is stored in 'buffer'
    bzero(buffer,256);
    if(readTextTCP(buffer, sizeof(buffer)-1, sockfd) <0){
        error("ERROR on reading");
    }
    cout << buffer << endl;
    
    //write to server
    bzero(buffer,256);
    strcpy(buffer, argv[2]);
    writeTextTCP(sockfd, buffer);

    buffer[strcspn(buffer, "\n")] = 0;

    //handle data transfer. Use ofstream (file output) to extract filename from buffer.
    const char *tempBuf = extractFileName(buffer);
    char tb[256];
    strncpy(tb, tempBuf, sizeof(tb));

    bzero(buffer,256);
    readTextTCP(buffer, sizeof(buffer), sockfd);
    sscanf(buffer, "%d", &size);

    if(size<=0){
        error("ERROR file does not exist");
    }

    ofstream outfile(tb, std::ios::binary);

    cout << "File size (int): " << buffer << endl;
    cout << "Getting file..." << endl;

    while(size > 0){
        n = read(sockfd, fbuf, (size > 1000 ? 1000 : size));
        if(n <= 0){
            error("ERROR reading from socket");
        }

        outfile.write((char*)fbuf, n);
        size-=n;

        cout << "Size left: " << size << "  Sending size: " << n << endl;
    }
    cout << "File transfer complete!" << endl;
    outfile.close();
    close(sockfd);
    return 0;
}