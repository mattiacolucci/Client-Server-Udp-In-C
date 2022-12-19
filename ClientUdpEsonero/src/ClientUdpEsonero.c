#include "libraryForSocketFunctions.h"

void readChoice(char* str);
char* dnsName(char* ip);
char* dnsAddr(char*name);

int main(int argc, char **argv) {
	#if defined WIN32   //if I am in windows
		WSADATA wsa_data; //I initialize the struct that let me use socket from library wsock32
		int result=WSAStartup(MAKEWORD(2,2),&wsa_data);
		if(result!=NO_ERROR){
			puts("error at WSAstartup");
			return -1;
		}

		//the library wsock32 is accessible
		puts("no error occurred");
	#endif

	char* serverIp;
	int serverPort;
	if(argc>1){  //if I passed more than 1 args
		char* domainName=argv[1];  //the server domainname and port is the second arg; the format is ip:port
		//tokenize the string to get the domainName and the port of the server
		char* token = strtok(domainName,":");
		sprintf(domainName,"%s",token);  //put the first part of the string passed as arg, and assign it to domainName var
		token=strtok(NULL,":");
		serverPort=atoi(token);  //the second part of the string is instead the server port
		serverIp=dnsAddr(domainName);
		if(serverIp==NULL){
			clearwinsock();
			return -1;
		}
	}else{
		serverIp=DEFAULT_IP;
		serverPort=DEFAULT_PORT;
	}

	int clientSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);  //create the socket of client; the socket function return an integer representing the socket created
	struct sockaddr_in fromAddr;
	int fromAddrLen=sizeof(fromAddr);
	if(clientSocket<0){
		puts("error in client socket creation");
		clearwinsock();
		return -1;
	}

	struct sockaddr_in serverAddress;  //struct that represents server's port and address
	memset(&serverAddress,0,sizeof(serverAddress));
	serverAddress.sin_family=AF_INET;  //setting server's protocol family
	serverAddress.sin_addr.s_addr=inet_addr(serverIp);  //setting server's ip address
	serverAddress.sin_port=htons(serverPort);  //setting server's port

	char recvBuff[BUFFERSIZE];  //receive buffer
	char choice[20];  //string representing user operation chosen

	puts("Insert '=' to exit\n");
	readChoice(choice);
	while(choice[0]!='='){  //cicle until user insert a string that begins with =
		if(sendMex(clientSocket,choice,&serverAddress)<0){  //function that send the string insert from the user at the server
			closeAll(clientSocket);
			return -1;
		}
		puts("send successfully");
		if(recvMex(clientSocket,recvBuff,&fromAddr,&fromAddrLen)<0){  //receive the operation's result from the server
			closeAll(clientSocket);
			return -1;
		}

		if(recvBuff[0]=='E'){  //if the server's response begins with "E", it indicates an error
			puts("Error during the operation:");
			printf("%s\n",recvBuff);
		}else{
			printf("Result received from the server %s, ip %s is: %s\n",dnsName(inet_ntoa(fromAddr.sin_addr)),inet_ntoa(fromAddr.sin_addr),recvBuff);
		}
		readChoice(choice);
	}
	if(sendMex(clientSocket,choice,&serverAddress)<0){  //function that send the string insert from the user at the server
		closeAll(clientSocket);
		return -1;
	}
	closeAll(clientSocket);
	return EXIT_SUCCESS;
}

void readChoice(char* str){
	do{
		puts("Insert the operation to do, with the following syntax:\noperator number1 number2\npossible operations are: + (sum), - (subtraction), / (division), x (moltiplication)");
		memset(str,0,20);
		fflush(stdin);
		gets(str);
	}while(strcmp(str,"")==0);
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

char* dnsAddr(char*name){
	struct hostent* host=gethostbyname(name);  //make dns request to get the ip associated to the domain name; this ip is written in the local file hosts
	if(host==NULL){
		puts("Error in dns request");
		clearwinsock();
		return NULL;
	}
	struct in_addr* ina=(struct in_addr*)host->h_addr_list[0];  //take the first ip associated to the hostname
	return inet_ntoa(*ina);
}
