/*
 * echoClient.c
 *
 *  Created on: 17/06/2014
 *      Author: utnso
 */
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
/*int mainasd(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	char server_reply[1025];
	char* message=malloc(1025);
	int valread;

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

	puts("Connected\n");
	if ((valread = read(sock, server_reply, 1024)) == 0) {
		puts("recv failed");

	}
	server_reply[valread] = '\0';
	puts(server_reply);

	//keep communicating with server
	while (1) {
	gets(message);
		//Send some data
		if (send(sock, message, 1024, 0) == 0) {
			puts("Send failed");
			break;
			return 1;
		}

		//Receive a reply from the server
		if ((valread = read(sock, server_reply, 1024)) == 0) {
			puts("recv failed");
			break;

		}
		server_reply[valread] = '\0';
		puts(server_reply);
	}
	close(sock);
	return 0;
}*/

int obtenerTamanoArchivo(FILE*);

int main(int argc, char **argv) {
	FILE *archivo;
	int sock;
	struct sockaddr_in server;
	char lineaLiteral[1024];
	int valread;
	char handshake[21] = "Soy un nuevo Programa";
	archivo = fopen(argv[1], "r");
	int* tamano = malloc(4);
	*tamano=obtenerTamanoArchivo(archivo);
	char* literal=malloc(*tamano);
	char server_reply[1024];
	fgets(lineaLiteral, 1024, archivo);
	strcpy (literal, lineaLiteral);
	while(! feof(archivo)){
			fgets(lineaLiteral, 1024, archivo);
			strcat (literal, lineaLiteral);
			}

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected\n");
	if (send(sock, handshake, 21, 0) == 0) {
					puts("Send failed");
					return 1;
				}
	if ((valread = read(sock, server_reply, 1024)) == 0) {
		puts("recv failed");

	}
	server_reply[valread] = '\0';
	puts(server_reply);

	//keep communicating with server
	if (send(sock, tamano, 4, 0) == 0) {
				puts("Send failed");
				return 1;
			}

	if (send(sock, literal, *tamano, 0) == 0) {
			puts("Send failed");
			return 1;
		}

	 //Receive a reply from the server
	/*	if ((valread = read(sock, server_reply, tamano)) == 0) {
			puts("recv failed");
			return 1;
		}
		puts(server_reply);*/

	close(sock);
	return 0;
}
/*
int enviarArchivoPorSocket(FILE* archivo){
	int serverSocket;

	crearSocketS(&serverSocket, PUERTO);
	printf("Ya voy a empezar \n");
	char message[PACKAGESIZE];
	int tamanoArchivo = sizeof(archivo);
	while(! feof(archivo)){
		fgets(message, PACKAGESIZE, archivo);
		send(serverSocket, (void*)strlen(message), 4, 0);
		if (! feof(archivo)) send(serverSocket, message, strlen(message), 0);
	};

	send(serverSocket, "Ya, estamos\n", 14,0);
	printf("Ya envie el ya estamos\n");
	close(serverSocket);
	return 0;

}

void enviarPorSocketAlKernel(char* buffer){
	printf("%s", buffer);
	return;
}

int main(int argc, char **argv){



//	Es necesario que le pasen el puerto, por alguna razon despues de un connect
//	tarda cierto tiempo para usar el mismo puerto
//	llamenlo por consola asi: "./interprete /rutaarchivo XXXX

	enviarArchivoPorSocket(archivo);
	fclose(archivo);

	return 0;
}*/

int obtenerTamanoArchivo(FILE* archivoAbierto){ //TODO
	fseek(archivoAbierto,0L,SEEK_END);
	int tamano = ftell(archivoAbierto);
	fseek(archivoAbierto,0L,SEEK_SET);
	return tamano;
}
