/*
 * bibliotecaSockets.c

 *
 *  Created on: 15/07/2014
 *      Author: utnso
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "bibliotecaSockets.h"

#define BACKLOG 10


int enviarConRazon(int socket, t_log* logs, int razon, t_paquete *pack){
	if (pack==NULL){
		pack=malloc(sizeof(t_paquete));
		pack->msj=malloc(1);
		pack->tamano=0;
	}
	int tamano=pack->tamano; //Esta linea les puede sonar a boludo pero es super necesaria
	t_paquete *header=serializar2(crear_nodoVar(&razon, 4), crear_nodoVar(&tamano, 4), 0);
	if(send(socket, header->msj, header->tamano, 0)<=0){
		log_error(logs, "La cabecera no se pudo enviar correctamente");
		close(socket);
		return 0;
	}
	if (pack->tamano!=4&&pack->tamano!=0){
		if (send(socket, pack->msj, pack->tamano, 0)<=0){
			log_error(logs, "El mensaje no se pudo enviar correctamente");
			close(socket);
			return 0;
		}
	}
	free(header->msj);
	free(header);
	free(pack->msj);
	free(pack);
	return 1;
}
t_buffer *recibirConBuffer(int socket, int *p_razon, t_log *logs){
	t_buffer *aux=malloc(sizeof(t_buffer));
	int valread;
	char *header=malloc(16);
	if ((valread=recv(socket, header, 16, MSG_WAITALL))<=0){
		if(valread){log_error(logs, "Hubo un error al recibir la cabecera");
		}
		close(socket);
		return NULL;
	}
	int tamano;
	desempaquetar2(header, p_razon, &tamano, 0);
	aux->size=tamano;
	//free(header);
	if (tamano!=4&&tamano!=0){
		char *msj=malloc(tamano);
		aux->mensaje=msj;
		if((valread=recv(socket, msj, tamano, MSG_WAITALL))<=0){
			if(valread){log_error(logs, "Hubo un error al recibir la cabecera");
					}
			close(socket);
			return NULL;
		}
		return aux;
	}else{
		return NULL;
	}
}
char *recibirConRazon(int socket, int *p_razon, t_log *logs){
	t_buffer *aux=recibirConBuffer(socket, p_razon, logs);
	return aux==NULL?NULL:aux->mensaje;
}

int crearServidor(char* puerto, t_log* logs){
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(NULL, puerto, &hints, &serverInfo)){
		log_error(logs, "Hubo un error al obtener la informacion del servidor");
		return -1;
	}
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (listenningSocket==-1){
		log_error(logs, "Hubo un error al crear el socket");
		return -1;
	}

	//Pequeño fragmento salvador que permite re-utilizar los puertos.
	int yes=1;
	if (setsockopt(listenningSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
		perror("setsockopt");
		exit(1);
	}
	//Vieron que era pequeño.

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

int conectarCliente (char* ip, char* puerto, t_log* logs) {
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
		return 0;
	}
	if (connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)){
		log_error(logs, "Error al conectar el socket");
		close(serverSocket);
		return 0;
	}
	freeaddrinfo(serverInfo);
	return serverSocket;
}
