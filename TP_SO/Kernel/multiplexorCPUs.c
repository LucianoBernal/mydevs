/*
 * multiplexorScripts.c
 *
 *  Created on: 20/06/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <parser/parser/metadata_program.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <string.h>   //strlen
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "PCPinterface.h"
#include <biblioteca_comun/Serializacion.h>
#include "syscall.h"
#include "PCP.h"

#define TRUE   1
#define FALSE  0
#define PORT 7004

typedef enum {
//	CONFIRMACION,
	NUEVO_PROGRAMA,
	SALIDA_POR_QUANTUM,
	SALIDA_POR_BLOQUEO,
	SALIDA_NORMAL,
	SALIDA_POR_SEMAFORO,
	SIG_INT,
	SIGURS_1,
	WAIT,
	SIGNAL,
	IMPRIMIR,
	IMPRIMIR_TEXTO,
	GRABAR_VALOR,
	OBTENER_VALOR

} codigos_mensajes;

void* atencionCPUs(void* sinParametro) {
	int opt = TRUE;
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30,
			activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;
//	int* tamano = malloc(4);

	char buffer[1025];  //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;

	//a message
	char *message = "Kernel";
	char handshake[4] = "CPU";


	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	//create a master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections , this is just a good habit, it will work without this
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
			sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *) &address, sizeof(address))
			< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);

	//try to specify maximum of 10 pending connections for the master socket
	if (listen(master_socket, 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while (TRUE) {
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//add child sockets to set
		for (i = 0; i < max_clients; i++) {
			//socket descriptor
			sd = client_socket[i];

			//if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);

			//highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}

		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL );

		if ((activity < 0) && (errno != EINTR)) {
			printf("select error");
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master_socket, &readfds)) {
			if ((new_socket = accept(master_socket,
					(struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
			printf(
					"New connectionHOLA , socket fd is %d , ip is : %s , port : %d \n",
					new_socket, inet_ntoa(address.sin_addr),
					ntohs(address.sin_port));
			char*cpu=malloc(4);
			if ((valread = recv(new_socket, cpu/*buffer*/, 4, 0)) < 0) {
				perror("recive");
				puts("nonononnonono");
				close(new_socket);
			}
			puts("asd");
			printf("\n");
			if (strncmp(cpu/*buffer*/, handshake, 4) != 0) {
				printf(
						"Host disconnected No es una CPU Valida ,socket fd is %d , ip is : %s , port : %d \n",
						new_socket, inet_ntoa(address.sin_addr),
						ntohs(address.sin_port));
				close(new_socket);
			} else {
				puts("el handsahke salio bien");
				//send new connection greeting message
				t_paquete *paqueteACPU = serializar2(crear_nodoVar(message,7),crear_nodoVar(&quantum,4),crear_nodoVar(&retardo,4),0);
				int* tamanioPaquete = malloc(4);
				*tamanioPaquete=paqueteACPU->tamano;
				if (send(new_socket, tamanioPaquete, 4, 0)){
					perror("No se envió el tamaño del paquete correctamente");
				}
				if (send(new_socket, paqueteACPU->msj, *tamanioPaquete, 0)==-1) {
					perror("No se envió el paquete correctamente");
				}

				puts("Welcome message sent successfully");

				//add new socket to array of sockets
				for (i = 0; i < max_clients; i++) {
					//if position is empty
					if (client_socket[i] == 0) {
						client_socket[i] = new_socket;
						printf("Adding to list of sockets as %d\n", i);

						break;
					}
				}

				nuevaCPU(sd); //TODO es una nueva CPU

			}
		}

		//else its some IO operation on some other socket :)
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds)) {
				//Check if it was for closing , and also read the incoming message
				if ((valread = read(sd, buffer, 1024)) == 0) { //FIXME
					//Somebody disconnected , get his details and print
					getpeername(sd, (struct sockaddr*) &address,
							(socklen_t*) &addrlen);
					printf(
							"Host disconnected: socket fd is %d , ip %s , port %d \n",
							sd, inet_ntoa(address.sin_addr),
							ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
					sem_wait(&semCPUDesconectadaMutex);
					idUltimaCPUDesconectada = sd;
					seDesconectoCPU(sd);
					sem_post(&semCPUDesconectadaMutex);

					//TODO Aca va lo que haces despues que una CPU se te desconecto,
					//como ser sacarla de tu lista de CPUs
				}

				//Echo back the message that came in
				else {
					char *header = malloc(16);
//					int resultado;
					int *razon = malloc(sizeof(int));
					int *tamanoMensaje = malloc(4);
					recv(sd, (void*) header, 16, 0);
					desempaquetar2(header, razon, tamanoMensaje, 0);
					char *mensaje = malloc(*tamanoMensaje);
					recv(sd, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);
					t_PCB* pcb;
					int tiempo, valor, tamano;
					char* dispositivoIO;
					char* texto;
					char* semaforo;
					char* id;
					switch (*razon) {
					case SALIDA_POR_QUANTUM: //El Programa salio del CPU por quantum
						desempaquetar2(mensaje, &pcb, 0);
						programaSalioPorQuantum(pcb, sd);
						break;
					case SALIDA_NORMAL: //El Programa termino normalmente
						desempaquetar2(mensaje, &pcb, 0);
						moverAColaExit(pcb, sd);
						break;
					case SALIDA_POR_BLOQUEO: //El Programa salio por bloqueo
						desempaquetar2(mensaje, &pcb, &tiempo, &dispositivoIO,
								0);
						programaSalioPorBloqueo(pcb, tiempo, dispositivoIO, sd);
						break;
					case SIGURS_1: //la CPU se desconectó CON SIGUSR
						desempaquetar2(mensaje, &pcb, 0);
						sem_wait(&semCPUDesconectadaMutex);
						idUltimaCPUDesconectada = sd;
						seDesconectoCPUSigusr(sd, pcb);
						sem_post(&semCPUDesconectadaMutex);
						break;
					case WAIT:
						desempaquetar2(mensaje, &tamano, &semaforo,&pcb, 0);
						sc_wait(semaforo, pcb, sd);
						break;
					case SIGNAL:
						desempaquetar2(mensaje, &semaforo, 0);
						sc_signal(semaforo, sd);
						break;
					case IMPRIMIR:
						//TODO
						break;
					case IMPRIMIR_TEXTO:
						desempaquetar2(mensaje, &texto, 0);
						sc_imprimirTexto(texto, sd);
						break;
					case GRABAR_VALOR:
						desempaquetar2(mensaje, &id, &valor, 0);
						sc_grabar_valor(id, valor);
						break;
					case OBTENER_VALOR:
						desempaquetar2(mensaje, &id, 0);
						sc_obtener_valor(id, sd);
						break;
					}

					// aca va el switch para saber porque volvio(pero no preguntas por
					//desconexion, eso ya se sabe de antes. MMM igual ojo, quizas si
					//porque quizas convenga que el que el que haga el close se el kernel
				}
			}
		}
	}
	return 0;
}

