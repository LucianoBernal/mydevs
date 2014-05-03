#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PUERTO "6667"
#define PACKAGESIZE 4096	// Define cual va a ser el size maximo del paquete a enviar

int enviarPorSocket(FILE* archivo){

	struct addrinfo hints;
	struct addrinfo *serverInfo;
	struct addrinfo *serverInfoL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(IP, PUERTO, &hints, &serverInfo);
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, "6668", &hints, &serverInfoL);
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);
	char message[PACKAGESIZE];

	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	do{
		fgets(message, PACKAGESIZE, archivo);
		send(serverSocket, message, strlen(message) + 1, 0);
	}while(! feof(archivo));

	send(serverSocket, "Ya, estamos", 12,0);
	//comienza el Handshake
	int listenningSocket;
	listenningSocket = socket(serverInfoL->ai_family, serverInfoL->ai_socktype, serverInfoL->ai_protocol);
	bind(listenningSocket,serverInfoL->ai_addr, serverInfoL->ai_addrlen);
	freeaddrinfo(serverInfoL);
	listen(listenningSocket, 5);
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);
	char package[PACKAGESIZE];
	recv(socketCliente,(void*) package, PACKAGESIZE, 0);
	close(socketCliente);
	close(listenningSocket);
	//Termina el Handshake


	close(serverSocket);
	return 0;
}

void enviarPorSocketAlKernel(char* buffer){
	printf("%s", buffer);
	return;
}

int main(int argc, char **argv){
FILE* archivo;

	//char ruta[73];
	//printf("escriba la direccion del script:");
	//scanf("%s", ruta);
	archivo = fopen(argv[1], "r");
	enviarPorSocket(archivo);
	return 0;
}

