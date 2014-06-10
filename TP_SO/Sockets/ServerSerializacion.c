
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

#define TAMANO_CABECERA 2

int main22(){
	int listenningSocket;
	char *puerto="6666";

	crearSocketL(&listenningSocket, puerto);
	listen(listenningSocket, 5);

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	char *mensaje=malloc(50), *header=malloc(10), tamano;
	t_queue *queue_mensaje = queue_create();
	queue_push(queue_mensaje, &tamano);
	//Por ahora no mando mas nada por la cabecera
	recv(socketCliente, (void*)header, TAMANO_CABECERA, MSG_WAITALL);
	Desempaquetar(header, queue_mensaje);
	//Aca irian todas las variables que esperamos ansiosamente (?
	queue_push(queue_mensaje, mensaje);
	recv(socketCliente, (void*)mensaje, tamano, MSG_WAITALL);
	Desempaquetar(mensaje, queue_mensaje);
	//Esto es solo porque es un string, quizas deberia mandar el /0 en estos casos.
	mensaje[tamano]=0;
	printf("El tamano del mensaje supersecreto era: %d\n", tamano);
	printf("El mensaje supersecreto era: %s\n", mensaje);
	return 0;
}
