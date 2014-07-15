/*
 * bibliotecaSockets.c

 *
 *  Created on: 15/07/2014
 *      Author: utnso
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>

#define BACKLOG 10

int crearServidor(char* puerto, t_log* logs){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP
	if (!getaddrinfo(NULL, puerto, &hints, &serverInfo)){
		log_error(logs, "Hubo un error al obtener la informacion del servidor");
		return -1;
	}
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (listenningSocket==-1){
		log_error(logs, "Hubo un error al crear el socket");
		return -1;
	}

	if (bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen)==-1){
		log_error(logs, "Hubo un error al bindear el socket");
		close(listenningSocket);
		return -1;
	}
	freeaddrinfo(serverInfo);
	if (listen(listenningSocket, BACKLOG)==-1){
		log_error(logs, "Hubo un error al escuchar el socket");
		close(listenningSocket);
		return -1;
	}
	return listenningSocket;
}

int aceptarConexion(int socket, t_log* logs) {
	int socketCliente;
	struct sockaddr_storage their_addr;
	socklen_t addr_size = addr_size = sizeof their_addr;
	socketCliente = accept(socket, (struct sockaddr *) &their_addr, &addr_size);
	if(socketCliente == -1){
		log_error(logs, "Error al aceptar la conexion");
	}
	return socketCliente;
}

int conectarCliente (char* ip, char* puerto, int logs) {
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, puerto, &hints, &serverInfo);
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (serverSocket==-1){
		log_error(logs, "Error al crear el socket");
		return -1;
	}
	if (!connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)){
		log_error(logs, "Error al conectar el socket");
		close(serverSocket);
		return -1;
	}
	freeaddrinfo(serverInfo);
	return serverSocket;
}
