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

int main(){
	int serverSocket;
	crearSocketS(&serverSocket, "6666");
	char *mensaje=malloc(50),*otromensaje=malloc(50);
	printf("Dame lo que queres que te mande:");
	gets(mensaje);//gets>>scanf
	printf("\n%s %d\n", mensaje, strlen(mensaje));
	//mando este bit de mas solo porque hablamos de un string
	//si no tiene el /0 del otro lado se complican las cosas
	t_paquete *paquete=serializar2(crear_nodoVar(mensaje, strlen(mensaje)+1), 0);
	t_paquete *header=serializar2(crear_nodoVar(&(paquete->tamano), 1), 0);
	desempaquetar2(paquete->msj, otromensaje, 0);
	printf("%s\n", otromensaje);
	send(serverSocket, header->msj, header->tamano, 0);
	send(serverSocket, paquete->msj, paquete->tamano, 0);
	printf("Ya te mande todo\n");
	return 0;
}

