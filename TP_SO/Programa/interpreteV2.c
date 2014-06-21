#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "bibSockets.h"
//Tuve que copiar tambien aca las librerias porque no se como llamarla
//cuando esta afuera de la carpeta
#define IP "127.0.0.1"
#define PACKAGESIZE 1024
#define PUERTO "8888"

int main(int argc, char **argv) {
		FILE *archivo;
		int sock;
		struct sockaddr_in server;
		char lineaLiteral[PACKAGESIZE];
		int valread;
		char handshake[21] = "Soy un nuevo Programa";
		archivo = fopen(argv[1], "r");
		int* tamano = malloc(4);
		*tamano=obtenerTamanoArchivo(archivo);
		char* literal=malloc(*tamano);
		char server_reply[1024];
		fgets(lineaLiteral, PACKAGESIZE, archivo);
		strcpy (literal, lineaLiteral);
		while(! feof(archivo)){
				fgets(lineaLiteral, PACKAGESIZE, archivo);
				strcat (literal, lineaLiteral);
				}
		fclose(archivo);

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
		if ((valread = read(sock, server_reply, PACKAGESIZE)) == 0) {
			puts("recv failed");

		}
		server_reply[valread] = '\0';
		puts(server_reply);

		//mandar literal
		if (send(sock, tamano, 4, 0) == 0) {
					puts("Send failed");
					return 1;
				}

		if (send(sock, literal, *tamano, 0) == 0) {
				puts("Send failed");
				return 1;
			}

		while(1){
			if ((valread = recv(sock, server_reply, PACKAGESIZE,MSG_WAITALL)) == 0) {
						puts("recv failed");
						break;

					}
			if ((valread = recv(sock, server_reply, PACKAGESIZE,MSG_WAITALL)) == 0) {
						puts("recv failed");
						break;
					}

		}


		close(sock);
		return 0;
	}
	int obtenerTamanoArchivo(FILE* archivoAbierto){
		fseek(archivoAbierto,0L,SEEK_END);
		int tamano = ftell(archivoAbierto);
		fseek(archivoAbierto,0L,SEEK_SET);
		return tamano;
	}
