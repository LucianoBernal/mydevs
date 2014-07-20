/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "atencionInterna.h"
#include "atencioninterna_interfaz.h"
#include <biblioteca_comun/Serializacion.h>
#include <biblioteca_comun/definiciones.h>

pthread_mutex_t mutexOperacion = PTHREAD_MUTEX_INITIALIZER;
//typedef enum {
//	Kernel = 1, CPU = 2
//} saludos_internos;

void* atencionInterna(void* sinParametro) {
	char* saludoKernel = malloc(7);
	char* saludoCpu = malloc(4);

	int socket = crearServidor(puertoUMV, logger);
	int socketKernel = aceptarConexion(socket, logger);

	while (1) {

		recv(socketKernel, (void*) saludoKernel, 30, 0);

		if (!strncmp(saludoKernel, "Kernel", 6)) {
			send(socketKernel, "UMV", 4, 0);
			//Creo hilo de atención al kernel.
			if (pthread_create(&hiloKernel, NULL, (void *) atencionKernel,
					(void*) &socketKernel)) {
				log_error(logger,
						"El hilo del Kernel no se creó correctamente.");
			} else {
				log_info(logger, "El hilo del Kernel se creó correctamente.");
			}
			break;
		} else {
			close(socketKernel);
		}
	} //Cierra while de Kernel

	while (1) {

		int socketCPU = aceptarConexion(socket, logger);
		cantCpu++;

		recv(socketCPU, (void*) saludoCpu, 30, 0);

		if (!strncmp(saludoCpu, "CPU", 3)) {
			send(socketCPU, "UMV", 4, 0);
			//Creo hilo de atención a CPU.
			if (pthread_create(&hiloCpu, NULL, (void *) atencionCpu,
					(void*) &socketCPU)) {
				log_error(logger, "El hilo de CPU no se creó correctamente.");
			} else {
				log_info(logger, "El hilo de CPU se creó correctamente.");
			}

		} else {
			close(socketCPU);
		}
	}			//Cierra while de CPU
}			//Cierra atencionInterna

void atencionKernel(int* socketKernel) {
	char *header = malloc(16);
	int j = 0;
	int *razon = malloc(4);
	int *tamanoMensaje = malloc(4);
	int procesoActivo = 0, parametro[3], basesLogicas[4];
	log_info(logger, "Entró a atencioKernel.");
	int *pid = malloc(sizeof(int));
	while (1) {

		j++;
		char *mensaje = recibirConRazon(*socketKernel, razon, logger);
		switch (*razon) {

		case CREAR_SEGMENTOS_PROGRAMA:
			puts("recibi crear segmentos programa");
			pthread_mutex_lock(&mutexOperacion);
			int *tamano1 = malloc(sizeof(int)), *tamano2 = malloc(sizeof(int)),
					*tamano3 = malloc(sizeof(int)), *tamano4 = malloc(
							sizeof(int));
			desempaquetar2(mensaje, tamano1, tamano2, tamano3, tamano4, 0);
			int i;
			basesLogicas[0] = crearSegmento(*tamano1);
			basesLogicas[1] = crearSegmento(*tamano2);
			basesLogicas[2] = crearSegmento(*tamano3);
			basesLogicas[3] = crearSegmento(*tamano4);
			for (i = 0; i < 4; i++) {
				printf("la base logica %d es %d\n", i, basesLogicas[i]);
				if (basesLogicas[i] == -1) {
					destruirTodosLosSegmentos();
					*razon = MEMORY_OVERLOAD;
					*tamanoMensaje = 0;
					break;
				}
				*razon = BASES_LOGICAS;
				*tamanoMensaje = 32;
			}
			pthread_mutex_unlock(&mutexOperacion);
			enviarConRazon(*socketKernel, logger, *razon,
					serializar2(crear_nodoVar(&basesLogicas[0], 4),
							crear_nodoVar(&basesLogicas[1], 4),
							crear_nodoVar(&basesLogicas[2], 4),
							crear_nodoVar(&basesLogicas[3], 4), 0));
			break;

		case DESTRUIR_SEGMENTOS:
			pthread_mutex_lock(&mutexOperacion);
			cambiarProcesoActivo(procesoActivo);
			destruirTodosLosSegmentos();
			pthread_mutex_unlock(&mutexOperacion);
			break;

		case ESCRIBIR_EN_UMV:
			printf("recibi escribir en umv\n");
			pthread_mutex_lock(&mutexOperacion);
			cambiarProcesoActivo(procesoActivo);

			char *buffer = malloc(80);
			desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],
					buffer, 0);
			printf("Los parametros que envio fueron:\n base= %d\n offset=%d\n tamano= %d\n buffer= %s\n", parametro[0], parametro[1], parametro[2], buffer);
			enviarUnosBytes(parametro[0], parametro[1], parametro[2], buffer);
			pthread_mutex_unlock(&mutexOperacion);
			free(buffer);
			break;

		case CAMBIAR_PROCESO_ACTIVO:
			printf("recibi cambiar proceso activo");
			pthread_mutex_lock(&mutexOperacion);
			int *pid2 = malloc(sizeof(int));
			desempaquetar2(mensaje, pid2, 0);
			procesoActivo = *pid2;
			printf("\nel valor del pid es: %d\n", *pid2);
			cambiarProcesoActivo(*pid2);
			pthread_mutex_unlock(&mutexOperacion);
			puts("termine de cambiar proceso activo");
			free(pid2);
			break;
		case CREAR_PROCESO_NUEVO:
			puts("recibi crear proceso nuevo");
			pthread_mutex_lock(&mutexOperacion);
			printf("el pid que recibi es:%d\n", *pid);
			desempaquetar2(mensaje, pid, 0);
			printf("el pid que recibi es:%d\n", *pid);
//		int * pid=malloc(sizeof(int));
//		*pid=parametro[0];
//		list_add(listaProcesos, pid);
			if (listaProcesos == NULL ) {
				printf("lista procesos era null");
				listaProcesos = list_create();
			}
			agregarProceso(*pid, 'c');
			puts("no mori aca");
//		procesoActivo = parametro[0];
//		cambiarProcesoActivo(parametro[0]);
			pthread_mutex_unlock(&mutexOperacion);
			break;
		}

		free(mensaje);
	}
	free(header);
	free(tamanoMensaje);
}
void atencionCpu(int *socketCPU) {
	log_info(logger, "entro a atencionCpu");

	int procesoActivo = 0, parametro[4];

	int *tamanoMensaje = malloc(4);
//	char *header = malloc(16);
	int * razon = malloc(sizeof(int));
	while(1){
//	recv(*socketCPU, (void*) header, 16, MSG_WAITALL);
//	desempaquetar2(header, razon, tamanoMensaje, 0);
//	char *mensaje = malloc(*tamanoMensaje);
//	recv(*socketCPU, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);
	char *mensaje=recibirConRazon(*socketCPU, razon, logger);
	switch (*razon) {

	case SOLICITAR_A_UMV:
		pthread_mutex_lock(&mutexOperacion);
		cambiarProcesoActivo(procesoActivo);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2], 0);
		printf("\nRecibi solicitar bytes, base: %d, offset: %d, y tamano: %d\n", parametro[0], parametro[1], parametro[2]);
		char *respuesta = solicitarBytes(parametro[0], parametro[1],
				parametro[2]);
		printf("Y el resultado es buffer=%s", respuesta);
		pthread_mutex_unlock(&mutexOperacion);
		if (!strcmp(respuesta, "error")) {
//			*razon = SEGMENTATION_FAULT;
//			*tamanoMensaje = 0;
//			t_paquete *varALiberar = (t_paquete *) serializar2(
//					crear_nodoVar(razon, 4), crear_nodoVar(tamanoMensaje, 4),
//					0);
//			send(*socketCPU, varALiberar->msj, 16, 0);
//			free(varALiberar);
			enviarConRazon(*socketCPU, logger, SEGMENTATION_FAULT, NULL);
			break;
		}
		*razon = RESPUESTA_A_SOLICITAR_BUFFER;
		*tamanoMensaje = parametro[2];
		printf("Me solicitaron un buffer, base %d, offset %d, y tamano %d \n", parametro[0], parametro[1], parametro[2]);
		printf("Y la respuesta es %s\n", respuesta);
//		t_paquete *paquete = (t_paquete *) serializar2(
//				crear_nodoVar(respuesta, *tamanoMensaje), 0);
//		t_paquete *header = (t_paquete *) serializar2(crear_nodoVar(razon, 4),
//				crear_nodoVar(&paquete->tamano, 4), 0);
//		send(*socketCPU, header->msj, 16, 0);
//		send(*socketCPU, paquete->msj, paquete->tamano, 0);
		enviarConRazon(*socketCPU, logger, RESPUESTA_A_SOLICITAR_BUFFER, serializar2(
				crear_nodoVar(respuesta, *tamanoMensaje), 0));
		free(respuesta);
		break;

	case ESCRIBIR_EN_UMV:
		pthread_mutex_lock(&mutexOperacion);
		cambiarProcesoActivo(procesoActivo);
		char *buffer = malloc(*tamanoMensaje);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],
				buffer, 0);
		int resultado = enviarUnosBytes(parametro[0], parametro[1], parametro[2], buffer);
		if (resultado){
			enviarConRazon(*socketCPU, logger, CONFIRMACION, NULL);
		}else{
			enviarConRazon(*socketCPU, logger, SEGMENTATION_FAULT, NULL);
		}
		pthread_mutex_unlock(&mutexOperacion);
		free(buffer);
		break;
	case CAMBIAR_PROCESO_ACTIVO:
		printf("recibi cambiar proceso activo");
		pthread_mutex_lock(&mutexOperacion);
		int *pid2 = malloc(sizeof(int));
		desempaquetar2(mensaje, pid2, 0);
		procesoActivo = *pid2;
		printf("\nel valor del pid es: %d\n", *pid2);
		cambiarProcesoActivo(*pid2);
		pthread_mutex_unlock(&mutexOperacion);
		puts("termine de cambiar proceso activo");
		free(pid2);
		break;
	}
	//free(header);
	free(mensaje);
	}
	free(tamanoMensaje);
	free(razon);
}

