/*
 * ClienteSerializacion.c
 *
 *  Created on: 09/06/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/collections/queue.h>
#include "bibSockets.h"
#include "Serializacion.h"

int main33(){
	int serverSocket;
	crearSocketS(&serverSocket, "6666");
	char *mensaje=malloc(50),*otromensaje=malloc(50);
	printf("Dame lo que queres que te mande:");
	gets(mensaje);//gets>>scanf
	printf("\n%s %d\n", mensaje, strlen(mensaje));
	t_queue *queue_mensaje =queue_create();
	//mando este bit de mas solo porque hablamos de un string
	//si no tiene el /0 del otro lado se complican las cosas
	queue_push(queue_mensaje, crear_nodoVar(mensaje, strlen(mensaje)+1));
	t_paquete *paquete=Serializar(queue_mensaje);
	queue_push(queue_mensaje, crear_nodoVar(&(paquete->tamano), 1));
	t_paquete *header=Serializar(queue_mensaje);
	queue_push(queue_mensaje, otromensaje);
	Desempaquetar(paquete->msj, queue_mensaje);
	printf("%s\n", otromensaje);
	send(serverSocket, header->msj, header->tamano, 0);
	send(serverSocket, paquete->msj, paquete->tamano, 0);
	printf("Ya te mande todo\n");
	return 0;
}

