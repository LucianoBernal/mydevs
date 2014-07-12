#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>

#define IP "127.0.0.1"
#define PACKAGESIZE 1024
#define PUERTO "8888"

int maino(t_log*, FILE*);
int main(int argc, char **argv) {
	t_log* logger;
	int fin;
	logger = log_create("logProcesoPrograma", "Programa", true, LOG_LEVEL_INFO);
	log_info(logger, "Comienza la ejecución del Programa.");
	FILE *archivo;
	archivo = fopen(argv[1], "r");
	if (NULL == archivo) {
		log_error(logger, "Hubo un error al abrir el Script");
		return EXIT_FAILURE;
	}
	if (maino(logger, archivo) == 1) {
		log_error(logger, "Finalizo Erroneamente la ejecucion del Programa");
		fin = EXIT_FAILURE;
	} else {
		log_info(logger, "Finalizo correctamente la ejecucion del Programa");
		fin = EXIT_SUCCESS;
	}
	log_destroy(logger);
	return fin;
}

int obtenerTamanoArchivo(FILE*);
int maino(t_log* logger, FILE* archivo) {
	int sock;
	struct sockaddr_in server;
	char lineaLiteral[PACKAGESIZE];
	int valread;
	char handshake[21] = "Soy un nuevo Programa";
	int* tamano = malloc(4);
	*tamano = obtenerTamanoArchivo(archivo);
	char* literal = malloc(*tamano);
	char server_reply[1024];
	fgets(lineaLiteral, PACKAGESIZE, archivo);
	strcpy(literal, lineaLiteral);
	while (!feof(archivo)) {
		fgets(lineaLiteral, PACKAGESIZE, archivo);
		strcat(literal, lineaLiteral);
	}
	fclose(archivo);
	log_info(logger,
			"Se concateno Script en buffer interno y se cerro archivo");
	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		log_error(logger, "No se pudo Crear el Socket");
		return 1;
	}
	log_info(logger, "Socket creado exitosamente");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		log_error(logger, "Fallo conexion al Kernel");
		return 1;
	}
	log_info(logger, "Conectado al Kernel...");
	log_info(logger, "Iniciando Handshake...");
	if (send(sock, handshake, 21, 0) == 0) {
		log_error(logger, "Fallo Send Handshake");
		return 1;
	}
	if ((valread = read(sock, server_reply, PACKAGESIZE)) == 0) {
		log_error(logger, "Fallo Recive Handshake");
		return 1;
	}
	server_reply[valread] = '\0';
	log_info(logger, server_reply);
	//mandar literal
	if (send(sock, tamano, 4, 0) == 0) {
		log_error(logger, "Fallo Send Literal");
		return 1;
	}

	if (send(sock, literal, *tamano, 0) == 0) {
		log_error(logger, "Fallo Send Literal");
		return 1;
	}
	free(literal);
	int fin=0;
	while (1) {
		if ((valread = recv(sock, tamano, 4, MSG_WAITALL)) == 0) {
			log_error(logger, "Fallo Recive");
			fin= 1;
			break;

		}
		char* respuesta = malloc(*tamano);
		if ((valread = recv(sock, respuesta, *tamano, MSG_WAITALL)) == 0) {
			log_error(logger, "Fallo Recive Handshake");
			fin= 1;
			break;
		}
		puts(server_reply);
		free(respuesta);

	}

	close(sock);
	free(tamano);
	return fin;
}
int obtenerTamanoArchivo(FILE* archivoAbierto) {
	fseek(archivoAbierto, 0L, SEEK_END);
	int tamano = ftell(archivoAbierto);
	fseek(archivoAbierto, 0L, SEEK_SET);
	return tamano;
}
