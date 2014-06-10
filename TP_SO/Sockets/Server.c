
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "bibSockets.h"

#define BACKLOG 5
#define PACKAGESIZE 1024
#define PUERTO "6660"

int main23(){
	int listenningSocket;

	crearSocketL(&listenningSocket, PUERTO);
	listen(listenningSocket, BACKLOG);

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	char package[PACKAGESIZE];
	int status = 1;
	int *tamano=malloc(sizeof(int));
	printf("Cliente conectado. Esperando mensajes:\n");

	while (status!=0){
		recv(socketCliente, (void*)tamano, 4, MSG_WAITALL);
		status = recv(socketCliente, (void*) package, *tamano, MSG_WAITALL);
		if ((status != 0)) printf("%s", package);
	}
	close(socketCliente);
	close(listenningSocket);
	return 0;
}
