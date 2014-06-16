/*
 * bibSockets2.c
 *
 *  Created on: 10/06/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
//La idea de hacer la libreria es empezar a controlar errores en las conexiones
//Algo asi como encanpsulamiento (?
void obtenerAddrInfoServer(char *ip, char *puerto, struct addrinfo **serverInfo){
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	getaddrinfo(ip, puerto, &hints, serverInfo);
}
void obtenerAddrInfoLocalHost(char *puerto, struct addrinfo **serverInfo){
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(NULL, puerto, &hints, serverInfo);
	//BLA
}

void escucharYCrearSocketCliente(int listenningSocket, int backlog, int *socketCliente, struct sockaddr_in * addr, socklen_t *addrlen){
	listen(listenningSocket, backlog);
	//tristemente se bloquea (?
	*socketCliente = accept(listenningSocket, (struct sockaddr *) addr, addrlen);
	//y creo que se bloquea otra vez.
}

void bindearSocket(int listenningSocket, struct addrinfo *serverInfo){
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
}
int crearSocket(struct addrinfo *serverInfo){
	return socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
}
void conectarSocket(int serverSocket, struct addrinfo *serverInfo){
	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
}
