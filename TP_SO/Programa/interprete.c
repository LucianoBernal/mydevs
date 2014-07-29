#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <biblioteca_comun/bibliotecaSockets.h>



#define PACKAGESIZE 1024


char* puerto_Kernel;
char* ip_Kernel;

int validarConfig(t_config*);
void cargarConfig(t_config*);
int programa(t_log*, FILE*);
int main(int argc, char **argv) {
	if (argc <= 1) {
			perror(
					"Se debe ingresar la dirección de un script\n");
			return EXIT_FAILURE;
		}

	char* rutaConfig = getenv("ANSISOP_CONFIG");
	t_config* config = config_create(rutaConfig);
	if (validarConfig(config)) {
			config_destroy(config);
			perror("El archivo de configuración no es correcto");
			return EXIT_FAILURE;
		}
	t_log* logger;
	int fin;
	logger = log_create("logProcesoPrograma", "Programa", true, LOG_LEVEL_DEBUG);
	log_info(logger, "Comienza la ejecución del Programa.");
	cargarConfig(config);
	FILE *archivo;
	archivo = fopen(argv[1], "r");
	if (NULL == archivo) {
		log_error(logger, "Hubo un error al abrir el Script");
		return EXIT_FAILURE;
	}
	if (programa(logger, archivo) == 1) {
		log_error(logger, "Finalizo Erroneamente la ejecucion del Programa");
		fin = EXIT_FAILURE;
	} else {
		log_info(logger, "Finalizo correctamente la ejecucion del Programa");
		fin = EXIT_SUCCESS;
	}
	fclose(archivo);
	log_debug(logger,"Se cerro el archivo Script");
	log_destroy(logger);
	return fin;
}

int obtenerTamanoArchivo(FILE*);
int programa(t_log* logger, FILE* archivo) {
	int sock;
	char lineaLiteral[PACKAGESIZE];
	int valread;
	char handshake[21] = "Soy un nuevo Programa";
	int tamano;
	tamano = obtenerTamanoArchivo(archivo);
	log_info(logger,"Obtuve tamanio del archivo,%d",tamano);
	char* literal = malloc(tamano);
	char server_reply[1024];
	fgets(lineaLiteral, PACKAGESIZE, archivo);
	strcpy(literal, lineaLiteral);
	while (!feof(archivo)) {
		fgets(lineaLiteral, PACKAGESIZE, archivo);
		strcat(literal, lineaLiteral);
	}
	literal[tamano-1] = 0;
	log_info(logger,
			"Se concateno Script en buffer interno y se cerro archivo");
	//Create socket
	if (!(sock=conectarCliente(ip_Kernel,puerto_Kernel,logger))){
		return 1;
	}
	log_info(logger, "Conectado al Kernel...");
	log_info(logger, "Iniciando Handshake...");
	if (send(sock, handshake, 21, 0) == -1) {
		log_error(logger, "Fallo Send Handshake");
		return 1;
	}
	if ((valread = recv(sock, server_reply, PACKAGESIZE,0)) == -1) {
		log_error(logger, "Fallo Recive Handshake");
		return 1;
	}
	server_reply[valread] = '\0';
	log_info(logger, server_reply);
	//mandar literal
	if (send(sock, &tamano, 4, 0) == -1) {
		log_error(logger, "Fallo Send Literal");
		return 1;
	}

	if (send(sock, literal, tamano, 0) == -1) {
		log_error(logger, "Fallo Send Literal");
		return 1;
	}
	puts(literal);
	free(literal);
	int fin=1;
	while (1) {
		/*
		if ((valread = recv(sock, tamano, 4, 0)) <=0) {
			log_error(logger, "Fallo Recive");
			fin= 1;
			break;

		}
		char* respuesta = malloc(*tamano);
		if ((valread = recv(sock, respuesta, *tamano, 0)) <= 0) {
			log_error(logger, "Fallo Recive Handshake");
			fin= 1;
			break;
		}
		respuesta[*tamano]=0;
		puts(respuesta);
		free(respuesta);
	*/
		int razon;
		t_buffer *buffer=recibirConBuffer(sock, &razon, logger);
		if (razon==SALIDA_NORMAL){
//			log_info(logger, "El programa finalizo normalmente");
			fin=0;
			break;
		}
		if (buffer==NULL/*&&razon!=SALIDA_NORMAL NO ESTARIA DE MAS, QUIZAS SI*/){
			log_error(logger, "Lucho editame, algo malo paso");
			fin=1;
			break;
		}
		char *texto=malloc(buffer->size);
		desempaquetar2(buffer->mensaje, texto, 0);
		texto[buffer->size-4]=0;
		puts(texto);
		free(texto);
		free(buffer->mensaje);
		free(buffer);
	}

	close(sock);
//	free(tamano);
	return fin;
}
int obtenerTamanoArchivo(FILE* archivoAbierto) {
	fseek(archivoAbierto, 0L, SEEK_END);
	int tamano = ftell(archivoAbierto);
	fseek(archivoAbierto, 0L, SEEK_SET);
	return tamano;
}

void cargarConfig(t_config *config) {
	puerto_Kernel = config_get_string_value(config, "PUERTO_KERNEL");
	ip_Kernel = config_get_string_value(config, "IP_KERNEL");
}

int validarConfig(t_config* config){
	if (!config_has_property(config, "PUERTO_KERNEL")) {
			perror("Falta PUERTO_KERNEL");
			return EXIT_FAILURE;
		}
		if (!config_has_property(config, "IP_KERNEL")) {
			perror("Falta IP_KERNEL");
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
}
