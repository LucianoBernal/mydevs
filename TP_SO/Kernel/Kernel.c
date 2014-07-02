/*
 * Kernel.c
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#include "PLP.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

void kernel_main() {
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
	return EXIT_SUCCESS;

	static t_queue* colaReady;
	colaReady = list_create();
	static sem_t * colaReadyMutex;
	sem_init(colaReadyMutex, 0, 1);
	static sem_t * vacioReady;
	sem_init(vacioReady, 0, 0);

	pthread_create()
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
	if (!config_has_property(config, "VALOR_SEMAFORO")) {
		perror("Falta VALOR_SEMAFORO");
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
	char *keyTamanio = "PUERTO_PROG";
	puerto_programa = config_get_int_value(config, keyPuerto_Prog);
	char *keyPUERTO_CPU = "PUERTO_CPU";
	puerto_CPU = config_get_int_value(config, keyPuerto_CPU);
	char *keyQUANTUM = "QUANTUM";
	quantum = config_get_int_value(config, keyQuantum);
	char *keyRETARDO = "RETARDO";
	retardo = config_get_int_value(config, keyRetardo);

	//TODO ME FALTA SEGUIR ACÁ
}

void* plp(void* parametro) {
	colaNew = list_create();
	randoms = list_create();
	sem_init(colaNuevosVacio, 0, 0);
	sem_init(randomMutex, 0, 1);
	sem_init(numABorrarMutex, 0, 1);
	sem_init(colaNuevosMutex, 0, 1);
}
