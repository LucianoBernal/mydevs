/*
 * umv.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "umv.h"

//pthread_mutex_t mutexCantProcActivos, mutexFlagCompactado, mutexAlgoritmo;

int main(int argc, char** argv) {

	//Verifico que se haya recibido por parámetro el archivo config.
	printf("%s", argv[1]);
	if (argc <= 1) {
		perror(
				"Se debe ingresar la dirección de un archivo de configuración\n");
		return EXIT_FAILURE;
	}

	t_config *config = config_create(argv[1]);
 	//t_config *config = config_create("/home/utnso/tp-2014-1c-skynet/TP_SO/UMV/Debug/config");
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

//	pthread_mutex_init(&mutexCantProcActivos, NULL);
//	pthread_mutex_init(&mutexFlagCompactado, NULL);
//	pthread_mutex_init(&mutexAlgoritmo, NULL);


	baseUMV = malloc(tamanoUMV);
	informarBaseUMV();
	listaProcesos = list_create();
	int* sinParametros=baseUMV;
	//Creo hilo que atiende a la consola.
	if (pthread_create(&atencion_consola, NULL, (void *) consola, (void*)sinParametros )) {
		log_info(logger, "Hubo un error en la creación del hilo de consola");
	} else {
		log_info(logger, "El hilo de consola se creó correctamente");
	}

	//Creo hilo que creará hilos que atienden al kernel/cpu's.
	if (pthread_create(&atencion_interna, NULL, (void *) atencionInterna,
			(void*)sinParametros )) {
		log_info(logger, "Hubo un error en la creación del hilo de atención interna");
	} else {
		log_info(logger, "El hilo de atención interna se creó correctamente");
	}
	pthread_join(atencion_consola,NULL);
	pthread_join(atencion_interna,NULL);
	log_debug(logger, "Voy a probar que no rompa antes");
	list_destroy_and_destroy_elements(listaProcesos, (void*)free);
	log_debug(logger, "Voy a probar que no rompa despues");
	config_destroy(config);
	log_destroy(logger);
	log_info(logger, "El proceso UMV ha finalizado");
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
	tamanoUMV = config_get_int_value(config, "TAMANIO_UMV");
	retardo = config_get_int_value(config, "RETARDO");
	algoritmo = config_get_int_value(config, "ALGORITMO");
	puertoUMV = config_get_string_value(config, "PUERTO");
}

