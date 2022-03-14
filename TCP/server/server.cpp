#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "iknlib.h"
#define SIZE 1000
using namespace std;

void sendFile(string fileName, long fileSize, int outToClient)
{
    
}


int main(int argc, char *argv[])
{
    //sockfd and newsockfd are file descriptors, portno stores server port number
    //clilen stores address of client and n stores read and write values.
    int sockfd, newsockfd, portno, n, w;
    socklen_t clilen;
    char buffer[256], fileBuffer[SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    FILE *fp;

    //checks for given port number. if invalid, print error code.
    if(argc < 2){
        error("ERROR port not provided");
    }

    //create new socket of IPv4 type, make it a TCP based stream and set protocol to '0'. This will
    //default to most appropriate protocol.
    //on error the socket returns -1. If this is the case, print errormessage.
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        error("ERROR opening socket");
    }

    //zeroes buffer
    bzero((char*) &serv_addr, sizeof(serv_addr));

    //portnumber is passed as second argument upon running server. 
    //this converts the ascii argument to an int
    portno = atoi(argv[1]);
    cout << "Port located: " << portno << endl;


    //serv add is a struct of type sockaddr_in in which sin_family contains a code for the address family.
    serv_addr.sin_family = AF_INET;

    //sin_port contains the port number, however, portno has to be converted to a network byte order in
    //order to be passed through as an argument.
    serv_addr.sin_port = htons(portno);

    //sin_addr contains s_addr which contains the IP address of the host server.
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    /*bind() binds a socket to its address and portnumber
    first sockfd is passed, secondly, a pointer to the struct sockaddr is passed, however, 
    what is really casted is the netinet struct sockaddr_in, therefore, it needs to be casted
    to properly reflect sockaddr serv_addr being a reference. Lastly, the length of the
    server address is passed. If bind return a -1, this means an error has occured.
    */
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }

    while(1){

        //listens on the socket for connections. first argument is the sockfd, the second is a wait-queue -
        //depicting number of connections that can be waiting while processing a connection.
        if(listen(sockfd,5) <0){
        error("ERROR on listen");
        }

        //the next few lines blocks until a client has connected to the server. here, much like the bind(),
        //we look instead at the client connecting to the server. the clients address is given as the second
        //argument and the length of the client address as the third argument. 
        clilen = sizeof(cli_addr);
        if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0){
            error("ERROR on accept");
        }

        //inet_ntoa concerts internet host address in to an IPv4 notation. ntohs converts network byte to host byte
        cout << "Connections established from " << inet_ntoa(cli_addr.sin_addr) << " port " << ntohs(cli_addr.sin_port) << endl;
        cout << "Listening for input from client..." << endl;
        
        //write to server
        bzero(buffer,256);
        writeTextTCP(newsockfd, "Hello from server");

        bzero(buffer,256);
        //reads text over TCP. Message is stored in 'buffer'
        if(readTextTCP(buffer, sizeof(buffer), newsockfd) <0){
            error("ERROR on reading");
        }

        //make filepath with NO \n
        cout << "Filepath given: " << buffer << endl;
        buffer[strcspn(buffer, "\n")] = 0;

        //Open the file given from client
        cout << "Opening file..." << endl;
        if ((fp = fopen(buffer, "r")) == NULL){
            error("Error on opening file");
        }
    
        //set start/end of the read file
        fseek(fp, 0, SEEK_END);

        //set file size to the ftell of the filepointer
        cout << "Getting file size..." << endl;
        long int size = ftell(fp);
        rewind(fp);

        //create array to send size of file
        char sizeArr[255];
        sprintf(sizeArr, "%ld", size);
        cout << "Sending file size..." << endl;
        writeTextTCP(newsockfd, sizeArr);

        //start transferring the file
        cout << "Starting file transfer..." << endl;
        size += 1000;
        while(size >0){
            if((n = fread(fileBuffer, 1, (size > 1000 ? 1000: size), fp)) < 0){
                error("Error in reading from socket");
            }
            if((n = write(newsockfd, fileBuffer, (size > 1000 ? 1000: size))) < 0){
                error("Error in writing to socket");
            }
            size-=1000;
        }
        bzero(fileBuffer, sizeof(fileBuffer));
        cout << "File transfered" << endl;

        cout << "" << endl;
        fclose(fp);
    }
    return 0;
}