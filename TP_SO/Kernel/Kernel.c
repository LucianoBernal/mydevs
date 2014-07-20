/*
* Kernel.c
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#include "Kernel.h"

int main(int argc, char** argv) {
	//Verifico que se haya recibido por parámetro el archivo config.
	if (argc <= 1) {
		perror(
				"Se debe ingresar la dirección de un archivo de configuración\n");
		return EXIT_FAILURE;
	}
	t_config *config = config_create(argv[1]);

	//Verifico que estén todos los valores esperados en el config.
	if (validarConfig(config)) {
		config_destroy(config);
		perror("El archivo de configuración no es correcto");
		return EXIT_FAILURE;
	}

	logKernel = log_create("logKERNEL", "Kernel", true, LOG_LEVEL_INFO);
	log_info(logKernel, "Comienza la ejecución del Kernel.");



	//Cargo parámetros del config en variables de Kernel.
	cargarConfig(config);
	variables_globales = dictionary_create();
	colaReady = queue_create();
	colaExit = queue_create();


	sem_init(&mostarColasMutex, 0, 1);
	sem_init(&colaExitMutex, 0, 1);
	sem_init(&colaExitVacio, 0, 0);
	sem_init(&mutexVG, 0, 1);
	sem_init(&grado_Multiprogramacion, 0, multiprogramacion);
	sem_init(&colaReadyMutex, 0, 1);
	sem_init(&vacioReady, 0, 0);
	socketUMV = conectarCliente(ip_UMV, puerto_UMV, logKernel);
	if(send(socketUMV,"Kernel",7,0)==-1){
		log_error(logKernel,"Fallo el Send del handshake");
		return EXIT_FAILURE;
	}
	char* buffer=malloc(4);
	if(recv(socketUMV,buffer,30,0)==-1){
		log_error(logKernel,"Fallo el Recive del handshake");
		return EXIT_FAILURE;
	};
	if (strncmp(buffer, "UMV", 3)){
		log_error(logKernel,"Fallo el Handshake con la UMV");
		return EXIT_FAILURE;
	}
	free(buffer);

	pthread_t plp, pcp;
	int iretPLP, iretPCP;
	int* parametrosPCP = NULL;
	iretPCP = pthread_create(&pcp, NULL, pcp_main, (void*) parametrosPCP);
	if (iretPCP) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iretPCP);
		exit(EXIT_FAILURE);
	}
	printf("Hilo pcp exitoso");
	int* parametrosPLP = NULL;
	iretPLP = pthread_create(&plp, NULL, plp_main, (void*) parametrosPLP);
	if (iretPLP) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iretPLP);
		exit(EXIT_FAILURE);
	}
	printf("Hilo plp exitoso");



//	pthread_join(pcp, NULL );
	pthread_join(plp, NULL );
	config_destroy(config);
	return EXIT_FAILURE;
}

int32_t validarConfig(t_config *config) {
	if (!config_has_property(config, "PUERTO_PROG")) {
		perror("Falta PUERTO_PROG");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "IP_UMV")) {
		perror("Falta IP_UMV");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "PUERTO_UMV")) {
		perror("Falta PUERTO_UMV");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "PUERTO_CPU")) {
		perror("Falta PUERTO_CPU");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "QUANTUM")) {
		perror("Falta QUANTUM");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "RETARDO")) {
		perror("Falta RETARDO");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "MULTIPROGRAMACION")) {
		perror("Falta MULTIPROGRAMACION");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "VALOR_SEMAFOROS")) {
		perror("Falta VALOR_SEMAFOROS");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "SEMAFOROS")) {
		perror("Falta SEMAFOROS");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "HIO")) {
		perror("Falta HIO");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "ID_HIO")) {
		perror("Falta ID_HIO");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "COMPARTIDAS")) {
		perror("Falta VARIABLES_GLOBALES");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "TAMANIO_STACK")) {
		perror("Falta TAMANIO_STACK");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

//SEMAFOROS=[b, c]
//VALOR_SEMAFOROS=[1, 0]
//ID_HIO=[HDD1, LPT1]
//HIO=[500, 200]
//COMPARTIDAS=[colas,compartida]

void cargarConfig(t_config *config) {
	puerto_programa = config_get_string_value(config, "PUERTO_PROG");
	ip_UMV = config_get_string_value(config, "IP_UMV");
	puerto_UMV = config_get_string_value(config, "PUERTO_UMV");
	puerto_CPU = config_get_string_value(config, "PUERTO_CPU");
	quantum = config_get_int_value(config, "QUANTUM");
	retardo = config_get_int_value(config, "RETARDO");
	multiprogramacion = config_get_int_value(config, "MULTIPROGRAMACION");
	tamanio_stack = config_get_int_value(config, "TAMANIO_STACK");
	valor_semaforos_aux = config_get_array_value(config, "VALOR_SEMAFOROS");
	semaforos_aux = config_get_array_value(config, "SEMAFOROS");
	hio_aux = config_get_array_value(config, "HIO");
	idhio_aux = config_get_array_value(config, "ID_HIO");
	variables_globales_aux = config_get_array_value(config, "COMPARTIDAS");
	/*int i=0;
	 while (idhio_aux[i] != NULL ) {
	 t_io* io;
	 io = malloc(sizeof(t_io));

	 io->nombre = dispositivos[i];
	 int a = atoi(dispRetardo[i]);
	 io->retardo = a;
	 io->cola = queue_create();
	 sem_init(&io->hayAlgo, 0, 0);
	 sem_init(&io->mutex, 0, 1);
	 dictionary_put(dispositivosIO, io->nombre, io);

	 pthread_t threadIO;
	 int hiloIO = pthread_create(&threadIO, NULL, manejoIO, (void*) io); //HILO PLP
	 i++;
	 if (hiloIO) {
	 log_error(logs, "Error en la creacion del hilo PLP");
	 log_destroy(logs);

	 exit(EXIT_FAILURE);
	 }*/

//	void cambiarTiposDeIO(){
//		int i=0;
//		cantidadDeDispositivos =0;
//			while (idhio_aux[i] != NULL ) {
//					list_add(idDispositivos, idhio_aux[i]);
//					list_add(retardos, &(atoi(hio_aux[i])));
//					i++;
//					cantidadDeDispositivos ++;
//					}
//	}
//	void cambiarTiposDeSemaforos(){
//	int i=0;
//	cantidadDeSemaforos = 0;
//		while (semaforos_aux[i] != NULL ) {
//				list_add(semaforos,semaforos_aux[i]);
//				lit_add(valor_semaforos, &(atoi(semaforos_aux[i])));
//				i++;
//				cantidadDeSemaforos ++;
//				}
//}
//	void cambiarTiposDeVariablesGlobales(){
//	int i=0;
//	variables_globales = dictionary_create();
//		while (variables_globales_aux[i] != NULL ) {
//				int a = 0;
//				dictionary_put(variables_globales,variables_globales_aux[i], &a);
//				i++;
//				}
//}
}
