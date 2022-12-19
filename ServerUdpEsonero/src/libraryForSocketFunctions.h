#ifndef LIBRARY_FOR_SOCKET_FUNCTIONS
#define LIBRARY_FOR_SOCKET_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define closesocket close
#endif

#define DEFAULT_IP "127.0.0.1"  //default ip address of server socket
#define DEFAULT_PORT 56700   //default port of the server, used by the clientto for connection
#define BUFFERSIZE 512  //server and client's receive buffer size

int sendMex(int,char*,struct sockaddr_in*);  //function that send the message to the other node that we are connected with, given int that represents own socket and the string to send
int recvMex(int,char*,struct sockaddr_in*,int*);  //function that receive a message, given own socket,the receive buffer and the struct sockaddr_in which will be filled with the sockAddr of the receiver
void closeAll(int);  //function that closes own socket and wsa
void clearwinsock();


#endif
