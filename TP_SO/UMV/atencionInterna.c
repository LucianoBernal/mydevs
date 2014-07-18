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
			send(socketKernel, "UMV",4,0);
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
	int j=0;
	int i, *razon=malloc(4);
	int *tamanoMensaje = malloc(4);
	int procesoActivo = 0, parametro[3], basesLogicas[3];
	log_info(logger, "Entró a atencioKernel.");
	int *pid=malloc(sizeof(int));
	while (1){

	j++;
	//*tamanoMensaje=10;

	recv(*socketKernel, (void*) header, 16, MSG_WAITALL);
	for(i=0; i<16;i++){
		printf("%c\n", header[i]==0?'0':header[i]);
	}
	printf("razon:%d\n",*razon);
	printf("tam mensaje:%d\n",*tamanoMensaje);
	desempaquetar2(header, (int*)razon, tamanoMensaje, 0);
	printf("razon:%d\n",*razon);
	printf("tam mensaje:%d\n",*tamanoMensaje);
	char *mensaje=malloc(*tamanoMensaje);
	printf("%d", j);
	recv(*socketKernel, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);

	switch (*razon) {

	case CREAR_SEGMENTOS_PROGRAMA:
		puts("recibi crear segmentos programa");
		pthread_mutex_lock(&mutexOperacion);
		int *tamano1=malloc(sizeof(int)), *tamano2=malloc(sizeof(int)), *tamano3=malloc(sizeof(int)), *tamano4=malloc(sizeof(int));
		desempaquetar2(mensaje, tamano1, tamano2, tamano3, tamano4, 0);
//		int i;
//		for (i = 0; i < 4; i++) {
//			resultado = crearSegmento(parametro[i]);
//			if (resultado == -1) {
//				*razon = MEMORY_OVERLOAD;
//				*tamanoMensaje = 0;
//				destruirTodosLosSegmentos();
//				break;
//			}
//			*razon = BASES_LOGICAS;
//			*tamanoMensaje = 32;
//			basesLogicas[i] = resultado;
//		}
		crearSegmento(*tamano1);
		crearSegmento(*tamano2);
		crearSegmento(*tamano3);
		crearSegmento(*tamano4);
		pthread_mutex_unlock(&mutexOperacion);
		t_paquete * aSerializarHeader = (t_paquete *)serializar2(crear_nodoVar(&razon, sizeof(razon)), crear_nodoVar(tamanoMensaje, 4), 0);
		t_paquete * aSerializarPaquete = (t_paquete *)serializar2(crear_nodoVar(&basesLogicas[0], 4),crear_nodoVar(&basesLogicas[1], 4),
				crear_nodoVar(&basesLogicas[2], 4),crear_nodoVar(&basesLogicas[3], 4), 0);
		send(*socketKernel,
				aSerializarHeader->msj, 16, 0);
		if (*tamanoMensaje) {
			send(*socketKernel,aSerializarPaquete->msj, 32, 0);
		}
		free(aSerializarHeader);
		free(aSerializarPaquete);
		break;

	case DESTRUIR_SEGMENTOS:
		pthread_mutex_lock(&mutexOperacion);
		cambiarProcesoActivo(procesoActivo);
		destruirTodosLosSegmentos();
		pthread_mutex_unlock(&mutexOperacion);
		break;

	case ESCRIBIR_EN_UMV:
		printf("recibi escribir en umv");
		pthread_mutex_lock(&mutexOperacion);
		cambiarProcesoActivo(procesoActivo);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],0);
		char *buffer = malloc(parametro[2]);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2], &buffer, 0);
		enviarUnosBytes(parametro[0], parametro[1], parametro[2], buffer);
		pthread_mutex_unlock(&mutexOperacion);
		free(buffer);
		break;

	case CAMBIAR_PROCESO_ACTIVO:
		printf("recibi cambiar proceso activo");
		pthread_mutex_lock(&mutexOperacion);
		int *pid2=malloc(sizeof(int));
		desempaquetar2(mensaje,pid2,0);
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
		desempaquetar2(mensaje,pid,0);
		printf("el pid que recibi es:%d\n", *pid);
//		int * pid=malloc(sizeof(int));
//		*pid=parametro[0];
//		list_add(listaProcesos, pid);
		if (listaProcesos==NULL){
			printf("lista procesos era null");
			listaProcesos=list_create();
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

	int procesoActivo = 0, parametro[3];

	int *tamanoMensaje = malloc(4);
	char *header = malloc(16);
	int * razon = malloc(sizeof(int));
	recv(*socketCPU, (void*) header, 16, 0);
	desempaquetar2(header, razon, tamanoMensaje, 0);
	char *mensaje= malloc(*tamanoMensaje);
	recv(*socketCPU, (void*) mensaje,*tamanoMensaje, MSG_WAITALL);
	switch(*razon){

		case SOLICITAR_A_UMV:
			pthread_mutex_lock(&mutexOperacion);
			cambiarProcesoActivo(procesoActivo);
			desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2], 0);
			char *respuesta=solicitarBytes(parametro[0], parametro[1], parametro[2]);
			pthread_mutex_unlock(&mutexOperacion);
			if (!strcmp(respuesta, "")){
				*razon = SEGMENTATION_FAULT;
				*tamanoMensaje = 0;
				t_paquete *varALiberar=(t_paquete *) serializar2(crear_nodoVar(razon,4), crear_nodoVar(tamanoMensaje,4), 0);
				send(*socketCPU, varALiberar->msj, 16, 0);
				free(varALiberar);
				break;
			}
			*razon = RESPUESTA_A_SOLICITAR_BUFFER;
			*tamanoMensaje = parametro[2];
			t_paquete *paquete = (t_paquete *)serializar2(crear_nodoVar(respuesta, *tamanoMensaje), 0);
			t_paquete *header = (t_paquete *)serializar2(crear_nodoVar(razon, 4), crear_nodoVar(&paquete->tamano, 4), 0);
			send(*socketCPU, header->msj, 16, 0);
			send(*socketCPU, paquete->msj , paquete->tamano, 0);
			free(paquete);
			free(header);
			free(respuesta);
			break;

		case ESCRIBIR_EN_UMV:
			pthread_mutex_lock(&mutexOperacion);
			cambiarProcesoActivo(procesoActivo);
			desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],0);
			char *buffer = malloc(parametro[2]);
			desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2], &buffer, 0);
			enviarUnosBytes(parametro[0], parametro[1], parametro[2], buffer);
			pthread_mutex_unlock(&mutexOperacion);
			free(buffer);
			break;

	}
	//free(header);
	free(mensaje);
	free(tamanoMensaje);
	free(razon);

}

