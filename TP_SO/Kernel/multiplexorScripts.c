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


#define TRUE   1
#define FALSE  0


void* atencionScripts(void* sinParametro) {
//int main() {
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30,
			activity, i, valread, sd;
	int max_sd;
	struct sockaddr_in address;
	int* tamano = malloc(4);
	char literal[1025];

	char buffer[1025];  //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;

	//a message
	char *message = "El programa se ha conectado al Kernel exitosamente \r\n";
	char handshake[21] = "Soy un nuevo Programa";

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	master_socket=crearServidor(puerto_programa,logKernel);

	printf("Esperando conexiones en el puerto %s:",puerto_programa);

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
					"New connection , socket fd is %d , ip is : %s , port : %d \n",
					new_socket, inet_ntoa(address.sin_addr),
					ntohs(address.sin_port));

			if ((valread =recv(new_socket, buffer, 21, 0)) < 0) {
				perror("recive");
				close(new_socket);
			}
			if (strncmp(buffer, handshake,21)!=0) {
				printf(
						"Host disconnected No es un Programa Valido ,socket fd is %d , ip is : %s , port : %d \n",
						new_socket, inet_ntoa(address.sin_addr),
						ntohs(address.sin_port));
						close(new_socket);
			}
			else{
			//send new connection greeting message
			if (send(new_socket, message, strlen(message), 0)
					!= strlen(message)) {
				perror("send");
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
			recv(new_socket, tamano, 4, MSG_WAITALL);
			literal[*tamano] =0;
			recv(new_socket, literal, *tamano, MSG_WAITALL);
			gestionarProgramaNuevo(literal,new_socket,*tamano);
			fflush(stdin);

		}
}

		//else its some IO operation on some other socket :)
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds)) {
				//Check if it was for closing , and also read the incoming message
				if ((valread = read(sd, buffer, 1024)) == 0) {
					//Somebody disconnected , get his details and print
					getpeername(sd, (struct sockaddr*) &address,
							(socklen_t*) &addrlen);
					printf("Host disconnected: socket fd is %d , ip %s , port %d \n",sd,
							inet_ntoa(address.sin_addr),
							ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
				}

				//Echo back the message that came in
				/*else {
				}*/
			}
		}
	}

	return 0;
}

