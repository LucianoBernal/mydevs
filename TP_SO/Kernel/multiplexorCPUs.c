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
#include "PCPinterface.h"
#include "Kernel.h"
#include "multiplexorCPUs_interfaz.h"
#include <biblioteca_comun/bibliotecaSockets.h>

extern char* puerto_CPU;
extern t_log *logKernel;
extern int quantum;
extern int retardo;
#define TRUE   1
#define FALSE  0



void* atencionCPUs(void* sinParametro) {
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30,
			activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;
//	int* tamano = malloc(4);

	char buffer[1025];  //data buffer of 1K

	//inicializo set
	fd_set readfds;


	char *message = "Kernel";
	char handshake[4] = "CPU";


	//inicializo todos los clientes en 0
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	master_socket=crearServidor(puerto_CPU,logKernel);

	log_info(logKernel,"Esperando conexiones de CPUs en el puerto: %s",puerto_CPU);
	while (TRUE) {
		//limpio el Set
		FD_ZERO(&readfds);

		//agrego master socket al set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//agrego resto de sockets al set para que sean detectados por el select
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			//si el sd es valido lo agrego al set
			if (sd > 0)
				FD_SET(sd, &readfds);

			//guardo el mayor numero de sd, para poner como parametro en el select
			if (sd > max_sd)
				max_sd = sd;
		}

		//espero por actividad en alguno de los sockets, timeout es NULL , espera indefinidamente
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL );

		if ((activity < 0) && (errno != EINTR)) {
			log_error(logKernel,"Error en el select");
		}

		//Si hubo actividad en el master socket, es una nueva conexion
		if (FD_ISSET(master_socket, &readfds)) {
			if ((new_socket = accept(master_socket,
					(struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
				log_error(logKernel,"Error al aceptar nueva conexion");
								exit(EXIT_FAILURE);
			}

			//informo nueva conexion
			log_info(logKernel,
								"Nueva Conexion de CPU , socket fd: %d , ip : %s , port: %d",
								new_socket, inet_ntoa(address.sin_addr),
								ntohs(address.sin_port));
			//handshake
			char*cpu=malloc(4);
			if ((valread = recv(new_socket, cpu/*buffer*/, 4, 0)) < 0) {
				log_error(logKernel,"Error en el recive del handshake");
				close(new_socket);
			}
			if (strncmp(cpu/*buffer*/, handshake, 4) != 0) {
				log_error(logKernel,"Handshake invalido");
								log_info(logKernel,
										"El Kernel desconecto a la CPU,no es una CPU Valida ,socket fd: %d , ip : %s , port: %d ",
										new_socket, inet_ntoa(address.sin_addr),
										ntohs(address.sin_port));
										close(new_socket);
										free(cpu);
			}
			else {
				free(cpu);
				//envio retardo y quantum a la CPU
				t_paquete *paqueteACPU = serializar2(crear_nodoVar(message,7),crear_nodoVar(&quantum,4),crear_nodoVar(&retardo,4),0);
				int* tamanioPaquete = malloc(4);
				*tamanioPaquete=paqueteACPU->tamano;
				if (send(new_socket, tamanioPaquete, 4, 0)){
					log_error(logKernel,"No se envió el tamaño del paquete correctamente");
				}
				if (send(new_socket, paqueteACPU->msj, *tamanioPaquete, 0)==-1) {
					log_error(logKernel,"No se envió el paquete correctamente");
				}

				log_info(logKernel,"Paquete con retardo y quantum enviado correctamente al sd: %d",new_socket);

				//agrego nuevo socket al vector de sockets
				for (i = 0; i < max_clients; i++) {

					if (client_socket[i] == 0) {
						client_socket[i] = new_socket;


						break;
					}
				}

				nuevaCPU(sd); //TODO es una nueva CPU

			}
		}

		//Hubo activdad en el resto de los sockets
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds)) {
				//Verifica si se cerro, y ademas lee el mensaje recibido
				if ((valread = read(sd, buffer, 1024)) == 0) { //FIXME
					//Alguna CPU se desconecto, obtengo la informacion
					getpeername(sd, (struct sockaddr*) &address,
							(socklen_t*) &addrlen);
					log_info(logKernel,"Una CPU se cerro abruptamente: socket fd: %d , ip: %s , puerto: %d \n",sd,
												inet_ntoa(address.sin_addr),
												ntohs(address.sin_port));

					//Cierra el socket y marca 0 el bit de ocupado
					close(sd);
					client_socket[i] = 0;
					sem_wait(&semCPUDesconectadaMutex);
					idUltimaCPUDesconectada = sd;
					seDesconectoCPU(sd);
					sem_post(&semCPUDesconectadaMutex);

					//TODO Aca va lo que haces despues que una CPU se te desconecto,
					//como ser sacarla de tu lista de CPUs
				}
				//Algun socket me envio algo, responder
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

