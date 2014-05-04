#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PUERTO "6667"
#define PACKAGESIZE 50

int enviarPorSocket(FILE* archivo){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(IP, PUERTO, &hints, &serverInfo);
	hints.ai_flags = AI_PASSIVE;

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);

	printf("Ya me voy a empezar \n");
	char message[PACKAGESIZE];

	do{
		fgets(message, PACKAGESIZE, archivo);
		send(serverSocket, message, strlen(message) + 1, 0);
	}while(! feof(archivo));

	send(serverSocket, "Ya, estamos\n", 14,0);
	printf("Ya envie el ya estamos\n");
	close(serverSocket);
	return 0;
}

void enviarPorSocketAlKernel(char* buffer){
	printf("%s", buffer);
	return;
}
/*
 * El problema que hay con esta version es que funciona bien, es decir,
 * envia el archivo completo solo en la primera corrida. Yo pienso que
 * hay una variable que no pierde el valor (probablemente archivo sigue
 * apuntando a eof) Soy Pato e_e.
 */
int main(int argc, char **argv){
FILE *archivo;

//	Tenemos que pasarle como parametro la ruta del archivo al momento
//	de ejecutarlo, es decir pongan ./interprete /home/(resto de la ruta)

	archivo = fopen(argv[1], "r");
	enviarPorSocket(archivo);
	close((int)archivo);

//	Las siguientes 3 lineas son intentos de solucionarlo (fallidos xd)

	fflush(stdin);
	fflush(stdout);
	free(archivo);
	return 0;
}

