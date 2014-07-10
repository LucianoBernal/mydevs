
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sockets/Serializacion.h>
#include <sockets/sockets.h>
#include <commons/config.h>

#define HANDSHAKE_SIZE 16
char *IP_UMV, *IP_KERNEL;
int PUERTO_UMV, PUERTO_PCP;

void obtenerDatosConfig(t_config *config){
	IP_UMV=config_get_string_value(config, "IP_UMV");
	IP_KERNEL=config_get_string_value(config, "IP_KERNEL");
	PUERTO_UMV=config_get_int_value(config, "PUERTO_UMV");
	PUERTO_PCP=config_get_int_value(config, "PUERTO_PCP");
	printf("datos levantados;\nIP_UMV=%s\nIP KERNEL=%s\nPUERTO UMV=%d\nPUERTO PCP=%d\n", IP_UMV, IP_KERNEL, PUERTO_UMV, PUERTO_PCP);
}
void cerrarCPU(t_log *log){
	log_error(log, "Deberia estar cerrando el CPU");
}
int main(int arc, char **argv) {
    t_log* log = log_create("/home/utnso/tp-2014-1c-skynet/TP_SO/CPU/Debug/log", "TEST",true, LOG_LEVEL_INFO);
	t_config *config=config_create(argv[1]);
	obtenerDatosConfig(config);
	int socketUMV, socketKernel;
	if (!(socketUMV = conectarCliente(IP_UMV, PUERTO_UMV, log))){
		log_error(log, "No se pudo conectar a la UMV exitosamente");
		cerrarCPU(log);
	}
	if(!(socketKernel = conectarCliente(IP_KERNEL, PUERTO_PCP, log))){
		log_error(log, "No se pudo conectar al Kernel exitosamente");
		cerrarCPU(log);
	}
	log_info(log, "ApruebenmeN pls");
	const char *saludo = "Holis soy un CPU";
	char* respuesta = malloc(8);
	int todoOk=0;
	send(socketUMV, saludo, 16, 0);
	recv(socketUMV, respuesta, 8, MSG_WAITALL);
	desempaquetar2(respuesta, &todoOk, 0);
	todoOk ?
			log_info(log, "El handshake con la UMV salio bien") :
			log_error(log, "El handshake con la UMV salio mal");
	send(socketKernel, saludo, 16, 0);
	recv(socketKernel, respuesta, 8, MSG_WAITALL);
	desempaquetar2(respuesta, &todoOk, 0);
	todoOk ?
			log_info(log, "El handshake con el Kernel salio bien") :
			log_error(log, "El handshake con el Kernel salio mal");
	//Inicio handshake
/*
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
	*/
	return 0;
}
