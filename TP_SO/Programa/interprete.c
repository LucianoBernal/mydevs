#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "bibSockets.h"
//Tuve que copiar tambien aca las librerias porque no se como llamarla
//cuando esta afuera de la carpeta
#define IP "127.0.0.1"
#define PACKAGESIZE 1024

int enviarArchivoPorSocket(FILE* archivo, char *PUERTO){
	int serverSocket;

	crearSocketS(&serverSocket, PUERTO);
	printf("Ya voy a empezar \n");
	char message[PACKAGESIZE];
	while(! feof(archivo)){
		fgets(message, PACKAGESIZE, archivo);
		send(serverSocket, (void*)strlen(message), 4, 0);
		if (! feof(archivo)) send(serverSocket, message, strlen(message), 0);
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
	char *PUERTO=argv[2];

//	Es necesario que le pasen el puerto, por alguna razon despues de un connect
//	tarda cierto tiempo para usar el mismo puerto
//	llamenlo por consola asi: "./interprete /rutaarchivo XXXX
	archivo = fopen(argv[1], "r");
	enviarArchivoPorSocket(archivo, PUERTO);
	fclose(archivo);

	return 0;
}

