#include "bibSockets2.c"
#include "Serializacion.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define HANDSHAKE_SIZE 16
char *IP_UMV, *PUERTO_UMV, *IP_KERNEL, *PUERTO_PCP;

int main() {
	//obtenerDatosConfig();
	//Inicio handshake
	struct addrinfo *addrInfo;
	obtenerAddrInfoServer(IP_UMV, PUERTO_UMV, &addrInfo);
	int socketUMV = crearSocket(addrInfo), todoOk, socketMio;
	conectarSocket(socketUMV, addrInfo);
	char *datosHandshake = malloc(HANDSHAKE_SIZE);
	send(socketUMV, "Hola soy un CPU", 15, 0);
	recv(socketUMV, datosHandshake, HANDSHAKE_SIZE, MSG_WAITALL);
	desempaquetar2(datosHandshake, &todoOk, 0);
	todoOk ?
			printf("Eeeexito en la conexion con la UMV") :
			printf("La UMV dice que sos un noob");
	obtenerAddrInfoServer(IP_KERNEL, PUERTO_PCP, &addrInfo);
	int socketPCP = crearSocket(addrInfo);
	conectarSocket(socketPCP, addrInfo);
	send(socketPCP, "Hola soy un CPU", 15, 0);
	recv(socketPCP, datosHandshake, HANDSHAKE_SIZE, 0);
	desempaquetar2(datosHandshake, &todoOk, &socketMio, 0);
	todoOk ? printf("Habemus kernel") : printf("Chuck testa");
	//Final handshake
	return 0;
}
