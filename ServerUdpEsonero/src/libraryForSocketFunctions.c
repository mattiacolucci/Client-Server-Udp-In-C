#include "libraryForSocketFunctions.h"

int sendMex(int sock, char* mex, struct sockaddr_in* destinationAddr){
	if(sendto(sock,mex,strlen(mex),0,(struct sockaddr*)destinationAddr,sizeof(*destinationAddr))!=strlen(mex)){  //se ho inviato un numero di buyte diverso da quello che intendevo inviare, allora si è verifiato un errore
		puts("send failed");
		return -1;
	}
	return 0;
}

int recvMex(int sock, char* buff, struct sockaddr_in* senderAddr, int* senderAddrLen){
	memset(buff,0,BUFFERSIZE);  //imposto il buffer per la ricezioni dei messaggi a \0, cioè al carattere di fine strinza
	int bytes_rcvd=recvfrom(sock,buff,BUFFERSIZE-1,0,(struct sockaddr*)senderAddr,senderAddrLen);  //ricevo il messaggio dal server; la funzione recv ritorna il numero di byte ricevuti, e concatena all'ultimo elemento del buffer, il messaggio ricevuto
	if(bytes_rcvd<=0){  //se i byte ricevuti sono 0 o meno, allora ce stato un errore
		puts("error in recv mex from server");
		return -1;
	}
	return 0;
}

void closeAll(int sock){
	closesocket(sock);
	clearwinsock();
}

void clearwinsock(){
	#if defined WIN32  //se compilo su windows
		WSACleanup();
	#endif
}
