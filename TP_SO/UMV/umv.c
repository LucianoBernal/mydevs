/*
 * umv.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "umv.h"

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

	//Cargo parámetros del config en variables de UMV.
	cargarConfig(config);
	//Creo Log.
	logger = log_create("logUmv", "UMV", true, LOG_LEVEL_INFO);
	log_info(logger, "Comienza la ejecución de la UMV.");

	baseUMV = malloc(tamanio_umv);
	listaProcesos = list_create();
	//Creo hilo que atiende a la consola.
	if (pthread_create(&atencion_consola, NULL, (void *) consola, NULL )) {
		//ERROR.
		log_info(logger, "Hubo un error en la creación del hilo de consola");
	} else {
		//Se creó hilo correctamente.
		log_info(logger, "El hilo de consola se creó correctamente");
	}

	//Creo hilo que creará hilos que atienden al kernel/cpu's.
	if (pthread_create(&atencion_interna, NULL, (void *) atencionInterna,
			NULL )) {
		//ERROR.
		log_info(logger, "Hubo un error en la creación del hilo de atención interna");
	} else {
		//Se creó correctamente.
		log_info(logger, "El hilo de atención interna se creó correctamente");
	}
	log_info(logger, "El proceso UMV ha finalizado");
	list_destroy_and_destroy_elements(listaProcesos, (void*)free);
	config_destroy(config);
	log_destroy(logger);
	return EXIT_SUCCESS;
}

int32_t validarConfig(t_config *config) {
	if (!config_has_property(config, "TAMANIO_UMV")) {
		perror("Falta TAMANIO_UMV");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "RETARDO")) {
		perror("Falta RETARDO");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "ALGORITMO")) {
		perror("Falta ALGORITMO");
		return EXIT_FAILURE;
	}
	if (!config_has_property(config, "PUERTO")) {
			perror("Falta PUERTO");
			return EXIT_FAILURE;
		}

	return EXIT_SUCCESS;
}

void cargarConfig(t_config *config) {
	char *keyTamanio = "TAMANIO_UMV";
	tamanio_umv = config_get_int_value(config, keyTamanio);
	char *keyRetardo = "RETARDO";
	retardo = config_get_int_value(config, keyRetardo);
	char *keyAlgoritmo = "ALGORITMO";
	algoritmo = config_get_int_value(config, keyAlgoritmo);
	char *keyPuerto = "PUERTO";
	puertoUMV = config_get_int_value(config, keyPuerto);
}

