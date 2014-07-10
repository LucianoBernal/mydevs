/*
 * Kernel.c
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#include "PLP.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <semaphore.h>
#include <pthread.h>

static int puerto_programa;
static int puerto_CPU;
static int quantum;
static int retardo;
static int multiprogramacion;
static t_queue* colaReady;
static t_dictionary* variables_globales;
static int semaforos[];
static char* valor_semaforos[];
static int hio[];
static char* idhio[];
sem_t* mutexVG = 1;

void cargarConfig(t_config *);
int32_t validarConfig(t_config*);
void obtener_valor(char id, int idCpu);
void grabar_valor(char id, int valor);
void wait(int idSem);
void signal(int idSem);

int kernel_main(int argc, char** argv) {
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

	//Cargo parámetros del config en variables de Kernel.
	cargarConfig(config);
	config_destroy(config);
	colaReady = queue_create();
	static sem_t * colaReadyMutex;
	sem_init(colaReadyMutex, 0, 1);
	static sem_t * vacioReady;
	sem_init(vacioReady, 0, 0);

	pthread_t plp, pcp;
	int iretPLP, iretPCP;

	iretPCP = pthread_create(&pcp, NULL, pcp_main, (void*) parametrosPCP); //TODO bely definir pcp_main y si lleva parametros
	if (iretPCP) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iretPCP);
		exit(EXIT_FAILURE);
	}
	printf("Hilo pcp exitoso");
	int* parametroPLP = NULL; //FIXME
	iretPLP = pthread_create(&plp, NULL, plp_main, (void*) parametroPLP);
	if (iretPLP) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iretPLP);
		exit(EXIT_FAILURE);
	}
	printf("Hilo plp exitoso");
	pthread_join(pcp, NULL );
	pthread_join(plp, NULL );
	return EXIT_FAILURE;
}

int32_t validarConfig(t_config *config) {
	if (!config_has_property(config, "PUERTO_PROG")) {
		perror("Falta PUERTO_PROG");
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
	if (!config_has_property(config, "VARIABLES_GLOBALES")) {
		perror("Falta VARIABLES_GLOBALES");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void cargarConfig(t_config *config) {
	char *keyPUERTO_PROG = "PUERTO_PROG";
	puerto_programa = config_get_int_value(config, keyPUERTO_PROG);
	char *keyPUERTO_CPU = "PUERTO_CPU";
	puerto_CPU = config_get_int_value(config, keyPUERTO_CPU);
	char *keyQUANTUM = "QUANTUM";
	quantum = config_get_int_value(config, keyQUANTUM);
	char *keyRETARDO = "RETARDO";
	retardo = config_get_int_value(config, keyRETARDO);
	char *keyMULTIPROGRAMACION = "MULTIPROGRAMACION";
	multiprogramacion = config_get_int_value(config, keyMULTIPROGRAMACION);
	char *keyVALOR_SEMAFOROS = "VALOR_SEMAFOROS";
	valor_semaforos = config_get_int_value(config, keyVALOR_SEMAFOROS);
	char *keySEMAFOROS = "SEMAFOROS";
	semaforos = config_get_int_value(config, keySEMAFOROS);
	char *keyHIO = "HIO";
	hio = config_get_int_value(config, keyHIO);
	char *keyIDHIO = "IDHIO";
	idhio= config_get_int_value(config, keyIDHIO);
	char *keyVARIABLES_GLOBALES = "VARIABLES_GLOBALES";
	variables_globales= config_get_int_value(config, keyVARIABLES_GLOBALES);
		}


void obtener_valor(char id, int idCpu) {
	sem_wait(mutexVG);
	int a = dictionary_get(variables_globales, &id);
	send(idCpu, a, strlen(a), 0);
	sem_wait(mutexVG);
}
void grabar_valor(char id, int valor) {
	sem_wait(mutexVG);
	dictionary_put(variables_globales, &id, &valor);
	sem_wait(mutexVG);
}

int buscarValorSemaforo(char* semaforo) {
	int i = 0;
	while (valor_semaforos[i] != semaforo) {
		i++;
	}
	return semaforos[i];
}
