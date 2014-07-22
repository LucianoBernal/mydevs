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
#include "PLPinterface.h"
#include <biblioteca_comun/bibliotecaSockets.h>
#include "multiplexorScripts_interfaz.h"
#include <commons/log.h>


extern char* puerto_programa;
extern t_log *logKernel;
#define TRUE   1
#define FALSE  0


void* atencionScripts(void* sinParametro) {
//int main() {
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30,
			activity, i, valread, sd, primer_Prog=1;
	int max_sd;
	struct sockaddr_in address;
	int* tamano = malloc(4);
	char literal[1025];

	char buffer[1025];  //data buffer de 1K

	//inicializo set
	fd_set readfds;


	char *message = "El programa se ha conectado al Kernel exitosamente \r\n";
	char handshake[21] = "Soy un nuevo Programa";

	//inicializo todos los clientes en 0
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	master_socket=crearServidor(puerto_programa,logKernel);

	log_info(logKernel,"Esperando conexiones de programas en el puerto: %s",puerto_programa);

	while (TRUE) {
		//limpio el Set
		FD_ZERO(&readfds);

		//agrego master socket al set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//agrego resto de sockets al set para que sean detectados por el select
		for (i = 0; i < max_clients; i++) {
			//socket descriptor
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
			if(primer_Prog){
				primer_Prog=0;
				log_info(logKernel,
									"Nueva Conexion de Programa , socket fd: %d",new_socket);
			}
			else{log_info(logKernel,
					"Nueva Conexion de Programa , socket fd: %d , ip : %s , port: %d",
					new_socket, inet_ntoa(address.sin_addr),
					ntohs(address.sin_port));
			}

			//Handshake
			if ((valread =recv(new_socket, buffer, 21, 0)) < 0) {
				log_error(logKernel,"Error en el recive del handshake");
				close(new_socket);
			}
			if (strncmp(buffer, handshake,21)!=0) {
				log_error(logKernel,"Handshake invalido");
				log_info(logKernel,
						"El Kernel desconecto al Programa,no es un Programa Valido ,socket fd: %d , ip : %s , port: %d ",
						new_socket, inet_ntoa(address.sin_addr),
						ntohs(address.sin_port));
						close(new_socket);
			}
			else{
			//envio mensaje de bienvenida
			if (send(new_socket, message, strlen(message), 0)
					!= strlen(message)) {
				log_error(logKernel,"Error al enviar mensaje de bienvenida");
			}

			log_info(logKernel,"Mensaje de bienvenida enviado correctamente al sd: %d",new_socket);

			//agrego nuevo socket al vector de sockets
			for (i = 0; i < max_clients; i++) {

				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					break;
				}
			}
			recv(new_socket, tamano, 4, MSG_WAITALL);
			recv(new_socket, literal, *tamano, MSG_WAITALL);
			literal[*tamano] =0;
			log_info(logKernel,"Se recibio Literal proveniente del sd: %d",new_socket);
			puts(literal);
			gestionarProgramaNuevo(literal,new_socket,*tamano);
			fflush(stdin);

		}
}

		//Hubo activdad en el resto de los sockets
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds)) {
				//Verifica si se cerro, y ademas lee el mensaje recibido
				if ((valread = read(sd, buffer, 1024)) == 0) {
					//Algun programa se desconecto, obtengo la informacion
					getpeername(sd, (struct sockaddr*) &address,
							(socklen_t*) &addrlen);
					log_info(logKernel,"Un programa se cerro: socket fd: %d , ip: %s , puerto: %d \n",sd,
							inet_ntoa(address.sin_addr),
							ntohs(address.sin_port));
					//Busco y borro PCB y Solicito destruccion de segmentos en la UMV
					//TODO

					//Cierra el socket y marca 0 el bit de ocupado
					close(sd);
					client_socket[i] = 0;
				}

				//Algun socket me envio algo, responder
				/*else {
				}*/
			}
		}
	}

	return 0;
}

