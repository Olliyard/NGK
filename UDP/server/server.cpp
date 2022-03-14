#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, length, n, portno;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[1024];

   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }

   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) <0){
       error("ERROR opening socket");
   }

    portno = atoi(argv[1]);
    cout << "Port located: " << portno << endl;
    length = sizeof(server);
    bzero((char*) &server, length);

    //serv add is a struct of type sockaddr_in in which sin_family contains a code for the address family.
    server.sin_family = AF_INET;

    //sin_port contains the port number, however, portno has to be converted to a network byte order in
    //order to be passed through as an argument.
    server.sin_port = htons(portno);

    //sin_addr contains s_addr which contains the IP address of the host server.
    server.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr *) &server, length) < 0){
        error("ERROR on binding");
    }

    fromlen = sizeof(struct sockaddr_in);
    while(1){
        if((n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen))<0){
            error("ERROR recvfrom");
        }
        write(1, "Received a datagram: ", 21);
        write(1, buf, n);
        if ((n = sendto(sock, "Got your message\n", 17, 0, (struct sockaddr *)&from, fromlen)) < 0){
            error("ERROR sendto");
        }

    }
    return 0;
}