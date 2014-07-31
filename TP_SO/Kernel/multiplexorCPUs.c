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
#include "syscallskynet.h"
#include "PCPinterface.h"
#include "Kernel.h"
#include "multiplexorCPUs_interfaz.h"
#include <biblioteca_comun/bibliotecaSockets.h>

extern int idUltimaCPUDesconectada;
extern char* puerto_CPU;
extern t_log *logKernel;
extern int quantum;
extern int retardo;
extern sem_t victimasMutex;
extern t_list* victimas;
extern t_dictionary *variables_globales;

#define TRUE   1
#define FALSE  0

void* atencionCPUs(void* sinParametro) {
	printf("multiplexor: diccionario de variables vale: %x\n",
			(u_int) variables_globales);
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30,
			activity, i, sd, primer_CPU = 1, primer_desconexion = 1;
	int max_sd;
	struct sockaddr_in address;
	char *header = malloc(16);
	int *razon = malloc(sizeof(int));
	int *tamanoMensaje = malloc(4);

	//inicializo set
	fd_set readfds;

	char *message = "Kernel";
	char handshake[4] = "CPU";

	//inicializo todos los clientes en 0
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	master_socket = crearServidor(puerto_CPU, logKernel);

	log_info(logKernel, "Esperando conexiones de CPUs en el puerto: %s",
			puerto_CPU);
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
			log_error(logKernel, "Error en el select");
		}

		//Si hubo actividad en el master socket, es una nueva conexion
		if (FD_ISSET(master_socket, &readfds)) {
			if ((new_socket = accept(master_socket,
					(struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
				log_error(logKernel, "Error al aceptar nueva conexion");
				exit(EXIT_FAILURE);
			}

			//informo nueva conexion
			if (primer_CPU) {
				primer_CPU = 0;
				log_info(logKernel, "Nueva Conexion de CPU , socket fd: %d",
						new_socket);
			} else {
				log_info(logKernel,
						"Nueva Conexion de CPU , socket fd: %d , ip : %s , port: %d",
						new_socket, inet_ntoa(address.sin_addr),
						ntohs(address.sin_port));
			}
			//handshake
			char*cpu = malloc(4);
			if ((recv(new_socket, cpu/*buffer*/, 4, 0)) <= 0) {
				log_error(logKernel, "Error en el recive del handshake");
				close(new_socket);
			}
			if (strncmp(cpu/*buffer*/, handshake, 4) != 0) {
				log_error(logKernel, "Handshake invalido");
				log_info(logKernel,
						"El Kernel desconecto a la CPU,no es una CPU Valida ,socket fd: %d , ip : %s , port: %d ",
						new_socket, inet_ntoa(address.sin_addr),
						ntohs(address.sin_port));
				close(new_socket);
				free(cpu);
			} else {
				free(cpu);
				//envio retardo y quantum a la CPU
				int* quantum2 = malloc(4);
				*quantum2 = quantum;
				int* retardo2 = malloc(4);
				*retardo2 = retardo;
				enviarConRazon(new_socket, logKernel, HANDSHAKE_CPU_KERNEL,
						serializar2(crear_nodoVar(message, 7),
								crear_nodoVar(quantum2, 4),
								crear_nodoVar(retardo2, 4), 0));
				log_info(logKernel,
						"Paquete con retardo y quantum enviado correctamente al sd: %d",
						new_socket);

				//agrego nuevo socket al vector de sockets
				for (i = 0; i < max_clients; i++) {

					if (client_socket[i] == 0) {
						client_socket[i] = new_socket;

						break;
					}
				}

				nuevaCPU(new_socket); //TODO es una nueva CPU
				free(quantum2);
				free(retardo2);

			}
		}

		//Hubo activdad en el resto de los sockets
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds)) {
				t_buffer* mensaje;
				//char* mensaje;

				puts("voy a recibir algo");
				mensaje = recibirConBuffer(sd, razon, logKernel);
				//Verifica si se cerro, y ademas lee el mensaje recibido
				if (mensaje == NULL
						&& *razon
								!= SIGUSR_1/*recv(sd, (void*) header, 16, 0) <= 0*/) {
					//Alguna CPU se desconecto, obtengo la informacion
					getpeername(sd, (struct sockaddr*) &address,
							(socklen_t*) &addrlen);
					if (primer_desconexion) {
						primer_desconexion = 0;
						log_info(logKernel,
								"Una CPU se cerro: socket fd: %d \n", sd);

					} else {

						log_info(logKernel,
								"Una CPU se cerro: socket fd: %d , ip: %s , puerto: %d \n",
								sd, inet_ntoa(address.sin_addr),
								ntohs(address.sin_port));
					}

					//Cierra el socket y marca 0 el bit de ocupado
					close(sd);
					client_socket[i] = 0;
					sem_wait(&semCPUDesconectadaMutex);
					idUltimaCPUDesconectada = sd;
					seDesconectoCPU(sd);
					sem_post(&semCPUDesconectadaMutex);
//					if(*razon==SIGUSR_1){
//						enviarConRazon(sd,logKernel,CONFIRMACION,NULL);
//					}

					//TODO Aca va lo que haces despues que una CPU se te desconecto,
					//como ser sacarla de tu lista de CPUs
				}
				//Algun socket me envio algo, responder
				else {
//					desempaquetar2(header, razon, tamanoMensaje, 0);
//					char *mensaje = malloc(*tamanoMensaje);
//					recv(sd, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);
					t_PCB* pcb = malloc(sizeof(t_PCB));
					char* pcbEmpaquetado = malloc(sizeof(t_PCB));
					int tiempo, valor, tamano;
					char* dispositivoIO = malloc(mensaje->size);
					char* texto = malloc(mensaje->size);
					char* semaforo = malloc(mensaje->size);
					char* id = malloc(mensaje->size);
					puts("recibi algo");
					switch (*razon) {
					case SALIDA_POR_QUANTUM: //El Programa salio del CPU por quantum
						puts("Salio por quantum");
						desempaquetarPCB(pcb, mensaje->mensaje);
						programaSalioPorQuantum(pcb, sd);
						break;
					case SALIDA_NORMAL: //El Programa termino normalmente
//						log_info(logKernel,"Aca estoy");
						desempaquetarPCB(pcb, mensaje->mensaje);
//						log_info(logKernel,"Aca NO estoy");
						moverAColaExityLiberarCPU(pcb, sd);
						break;
					case SALIO_POR_IO: //El Programa salio por bloqueo
						printf("Fuck yeah sali por IO\n");
//						desempaquetar2(mensaje, &pcb, &tiempo, &dispositivoIO,
//								0);
						desempaquetar2(mensaje->mensaje, pcbEmpaquetado,
								dispositivoIO, &tiempo, 0);
						desempaquetarPCB(pcb, pcbEmpaquetado);
						dispositivoIO[mensaje->size - 88] = 0;
						printf(
								"el dispositivo que recibi: %s y por un tiempo: %d\n",
								dispositivoIO, tiempo);
						programaSalioPorBloqueo(pcb, tiempo, dispositivoIO, sd);
						break;
					case SALIDA_POR_SEMAFORO:
//						desempaquetar2(mensaje, pcbEmpaquetado, semaforo, 0);
//						desempaquetarPCB(pcb, pcbEmpaquetado);
//
						break;
					case SIGUSR_1: //la CPU se desconectó CON SIGUSR

//						desempaquetarPCB(pcb, mensaje->mensaje);
						log_info(logKernel,
								"Recibi señal SIGUSR1 para la CPU cuyo sd es:%d",
								sd);
//						close(sd);
//						client_socket[i] = 0;
						sem_wait(&semCPUDesconectadaMutex);
						idUltimaCPUDesconectada = sd;
						seDesconectoCPUSigusr(sd, pcb);
						sem_post(&semCPUDesconectadaMutex);
						break;
					case WAIT:
						puts("Me llego wait lean careta");
						desempaquetar2(mensaje->mensaje, semaforo, 0);
						semaforo[mensaje->size - 4] = 0;
						puts(semaforo);
						sc_wait(semaforo, sd);
						break;
					case SIGNAL:
						puts("olvis que Me llego signal");
						desempaquetar2(mensaje->mensaje, semaforo, 0);

						semaforo[mensaje->size - 4] = 0;
						puts(semaforo);
						sc_signal(semaforo, sd);
						break;
					case IMPRIMIR:
						//TODO
						break;
					case IMPRIMIR_TEXTO:
						printf("Ese algo era imprimir texto %d\n",
								mensaje->size);

						desempaquetar2(mensaje->mensaje, texto, 0);
						texto[mensaje->size - 4] = 0;
//						printf("No la cagaste desempaquetando %d\n", strlen(texto));
						sc_imprimirTexto(texto, sd);
						break;
					case GRABAR_VALOR:
						puts("Me llego grabar valor");
						desempaquetar2(mensaje->mensaje, id, &valor, 0);

						id[mensaje->size - 12] = 0;
						printf("El tamano de la cadena id es: %d\n",
								strlen(id));
//						printf("El nombre del semaforo es %s y el valor a grabar es %d\n", id, valor);
						sc_grabar_valor(id, valor, sd);
						break;
					case OBTENER_VALOR:
						puts("me llego obtener valor");
						desempaquetar2(mensaje->mensaje, id, 0);
						id[mensaje->size - 4] = 0;
						printf("El tamano de la cadena id es: %d\n",
								strlen(id));
						sc_obtener_valor(id, sd);
						break;
					}
					//	free( dispositivoIO);
					free(texto);
					//	free(semaforo);
					//	free(id);
					//	free(pcbEmpaquetado);

					// aca va el switch para saber porque volvio(pero no preguntas por
					//desconexion, eso ya se sabe de antes. MMM igual ojo, quizas si
					//porque quizas convenga que el que el que haga el close se el kernel
				}
				//free(mensaje->mensaje);
				//free(mensaje);
			}
		}
	}
	free(message);
	return 0;
}

