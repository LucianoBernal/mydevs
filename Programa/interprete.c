#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PACKAGESIZE 1024

char *PUERTO;

int enviarPorSocket(FILE* archivo){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(IP, PUERTO, &hints, &serverInfo);

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	printf("Ya voy a empezar \n");
	char message[PACKAGESIZE];
	while(! feof(archivo)){
		fgets(message, PACKAGESIZE, archivo);
		if (! feof(archivo)) send(serverSocket, message, strlen(message) + 1, 0);
	};

	send(serverSocket, "Ya, estamos\n", 14,0);
	printf("Ya envie el ya estamos\n");
	close(serverSocket);
	return 0;
}

void enviarPorSocketAlKernel(char* buffer){
	printf("%s", buffer);
	return;
}

int main(int argc, char **argv){

	FILE *archivo;
	PUERTO=argv[2];

//	Es necesario que le pasen el puerto, por alguna razon despues de un connect
//	tarda cierto tiempo para usar el mismo puerto
//	llamenlo por consola asi: "./interprete /rutaarchivo XXXX
	archivo = fopen(argv[1], "r");
	enviarPorSocket(archivo);
	fclose(archivo);

	return 0;
}

