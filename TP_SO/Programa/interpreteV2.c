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
#include <sys/stat.h>


#define PACKAGESIZE 1024


char* puerto_Kernel;
char* ip_Kernel;

char* obtener_programa(char* );
int validar_Config(t_config*);
void cargar_Config(t_config*);
int programaPosta(t_log*,char*);
int main2(int argc, char **argv) {
	if (argc <= 1) {
			perror(
					"Se debe ingresar la dirección de un script\n");
			return EXIT_FAILURE;
		}

	char* rutaConfig = getenv("ANSISOP_CONFIG");
	t_config* config = config_create(rutaConfig);
	if (validar_Config(config)) {
			config_destroy(config);
			perror("El archivo de configuración no es correcto");
			return EXIT_FAILURE;
		}
	t_log* logger;
	int fin;
	logger = log_create("logProcesoPrograma", "Programa", true, LOG_LEVEL_DEBUG);
	log_info(logger, "Comienza la ejecución del Programa.");
	cargar_Config(config);
	if (programaPosta(logger,argv[1]) == 0) {
		log_error(logger, "Finalizo Erroneamente la ejecucion del Programa");
		fin = EXIT_FAILURE;
	} else {
		log_info(logger, "Finalizo correctamente la ejecucion del Programa");
		fin = EXIT_SUCCESS;
	}
	log_debug(logger,"Se cerro el archivo Script");
	log_destroy(logger);
	return fin;
}


int programaPosta(t_log* logger,char* ruta) {
	int sock;
	int valread;
	char handshake[21] = "Soy un nuevo Programa";
	int tamano;
	log_info(logger,"Obtuve tamanio del archivo,%d",tamano);
	char server_reply[1024];
	char* literal =obtener_programa(ruta);
	tamano= strlen(literal) + 1;
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
		if (buffer==NULL&&razon!=IMPRIMIR_TEXTO/* NO ESTARIA DE MAS, QUIZAS SI*/){
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
	return fin;
}
int obtenerTamanoArchivo2(FILE* archivoAbierto) {
	fseek(archivoAbierto, 0L, SEEK_END);
	int tamano = ftell(archivoAbierto);
	fseek(archivoAbierto, 0L, SEEK_SET);
	return tamano;
}

void cargar_Config(t_config *config) {
	puerto_Kernel = config_get_string_value(config, "PUERTO_KERNEL");
	ip_Kernel = config_get_string_value(config, "IP_KERNEL");
}

int validar_Config(t_config* config){
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


char* obtener_programa(char* ruta){
	struct stat stat_file;
	stat(ruta, &stat_file);
	char* buffer;
	FILE* archi = NULL;
	archi = fopen(ruta, "r");
	if (archi != NULL) {
		buffer = calloc(1, stat_file.st_size + 1);
		fread(buffer, stat_file.st_size, 1, archi);
	}
	fclose(archi);
	return buffer;
}
