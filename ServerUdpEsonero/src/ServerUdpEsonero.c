#include "libraryForSocketFunctions.h"

#define QLEN 5 //number of simultaneous connection that the server is able to handle

int handleClient(int);
double add(int a,int b);
double division(int a,int b);
double sub(int a,int b);
double mult(int a,int b);
char* dnsName(char* name);

int main(void) {
	//Winsock initialization
	#if defined WIN32
		WSADATA wsa_data;
		int result=WSAStartup(MAKEWORD(2,2),&wsa_data);
		if(result!=NO_ERROR){
			puts("error at WSAstartup");
			return -1;
		}

		//winsock library is accessible
		puts("no error occurred");
	#endif
	int mySocket;   //variable that is going to contain an int that refers to the server welcome socket
	mySocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);  //create the server's welcome socket

	if(mySocket<0){  //if the int returned by the creation of the socket is <0, an error has been occurred
		puts("error while creating socket");
		return -1;
	}

	struct sockaddr_in socketAddress;  //struct that allows ip and port memorization; it's used to represent ip and port the server has to bind with
	memset(&socketAddress,0,sizeof(socketAddress));
	socketAddress.sin_family=AF_INET;
	socketAddress.sin_port=htons(DEFAULT_PORT);  //server's welcome socket's port, expressed by a function that converts the port number from local machine's endianness to the network one
	socketAddress.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(bind(mySocket,(struct sockaddr*)&socketAddress,sizeof(socketAddress))<0){  //bind between server's welcome socket and port that it has to listen on
		puts("bind failed");
		closeAll(mySocket);
		return -1;
	}

	puts("bind successfull");

	while(1){
		if(handleClient(mySocket)<0){
			closeAll(mySocket);
			return -1;
		}
		//the server will remain in an infinite loop, continuing listening for other new requests
	}

	closeAll(mySocket);
	return EXIT_SUCCESS;
}

int handleClient(int sock){  //functions that handle the communication with the client
	struct sockaddr_in clientAddr;
	int clientAddrLen=sizeof(clientAddr);
	char rcvdBuff[BUFFERSIZE];
	memset(rcvdBuff,0,BUFFERSIZE);
	memset(&clientAddr,0,sizeof(clientAddr));

	puts("waiting for requests...");

	if(recvMex(sock,rcvdBuff,&clientAddr,&clientAddrLen)<0){  //receive mex from the client
		closeAll(sock);
		return -1;
	}
	//the message will follow the following pattern:  <operator> <operand1> <operand2>
	//if operator is =, we return 0 since the communication is over
	while(rcvdBuff[0]!='='){
		printf("Requested the following operation %s from client %s, ip %s\n",rcvdBuff,dnsName(inet_ntoa(clientAddr.sin_addr)),inet_ntoa(clientAddr.sin_addr));

		char *token;
		char operator;
		int num1,num2;
		double result=0;
		int error=0;
		char* errorString;
		int count=0;

		//I will split the string by the space character, and thanks to a counter variable, I will store different parts of the string in different variables
		token = strtok(rcvdBuff, " ");
		while( token != NULL ) {
			switch(count){
				case 0:  //if counter is 0, token var contains the operator
					operator=token[0];
					break;
				case 1:  //if count is 1, token will contain the first operand, stored in a int variable
					if((num1=atoi(token))==0){  //if the conversion from string to integer goes wrong I take note of the error with a variable and set the error string
						error=1;
						errorString="E: not integer number or 0";
					}
					break;
				case 2:  //if count is 1, token will contain the second operand, stored in a int variable
					if((num2=atoi(token))==0){
						error=1;
						errorString="E: not integer number or 0";
					}
					break;
				default:  //if counter assume others values, so there are more spaces in the string and so its format is not correct
					error=1;
					errorString="E: Invalid string format";
					break;
			}
			count++;  //at every iteration I increment the counter
			token = strtok(NULL, " ");  //assign at token the other part of the string split by space
		}

		if(count==3){  //if server has found 3 parts in the string, separated by spaces, the string format is correct
			//printf("the operation is: %c %d %d\n",operator,num1,num2);

			//based on the operator took from the string, server will make 4 different operations, by calling functions, and storing them results on a double var
			switch(operator){
				case '+':
					result=add(num1,num2);
					break;
				case '-':
					result=sub(num1,num2);
					break;
				case 'x':
					result=mult(num1,num2);
					break;
				case '/':  //if the operand is /
					if(num2==0){  //if the denominator is 0,server will return an error
						error=1;
						errorString="E: Division by 0";
					}else{
						result=division(num1,num2);
					}
					break;
				default:  //if operator was none of the previous options, it's incorrect
					error=1;
					errorString="E: Invalid operator";
					break;
			}
		}else{
			error=1;
			errorString="E: Invalid string format";
		}

		char resultString[20];  //string that will contain the result of the operation made
		if(error==0){ //if there are no errors I send the result string to the client
			sprintf(resultString,"%d %c %d = %.2lf",num1,operator,num2,result);  //store in the string, the result var
			printf("the result of the operation is: %s\n",resultString);
		}else{  //if there is an error, the string to send is the errorString
			strcpy(resultString,errorString);
		}

		if(sendMex(sock,resultString,&clientAddr)<0){  //send the string to the client
			closeAll(sock);
			return -1;
		}

		if(recvMex(sock,rcvdBuff,&clientAddr,&clientAddrLen)<0){  //receive mex from the client
			closeAll(sock);
			return -1;
		}

	}
	return 0;
}

double add(int a,int b){
	return (double)a+b;
}

double mult(int a,int b){
	return (double)a*b;
}

double sub(int a,int b){
	return (double)a-b;
}

double division(int a,int b){
	return (double)a/b;
}

//function that returns the domainName associated at one ip given in input
char* dnsName(char* ip){
	struct in_addr addr;
	struct hostent *host;
	addr.s_addr = inet_addr(ip);
	host = gethostbyaddr((char *) &addr, 4, AF_INET);
	char* canonical_name = host->h_name;
	return canonical_name;
}
