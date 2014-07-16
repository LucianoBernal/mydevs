/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "atencionInterna.h"
#include "atencioninterna_interfaz.h"
#include <biblioteca_comun/Serializacion.h>
pthread_mutex_t mutexOperacion = PTHREAD_MUTEX_INITIALIZER;
typedef enum {
	Kernel = 1, CPU = 2
} saludos_internos;

typedef enum {
	BASES_LOGICAS,
	CONFIRMACION,
	SEGMENTATION_FAULT,
	MEMORY_OVERLOAD,
	MOSTRAR_VALOR,
	MOSTRAR_TEXTO,
	CREAR_SEGMENTO,
	CREAR_SEGMENTOS_PROGRAMA,
	DESTRUIR_SEGMENTOS,
	ESCRIBIR_EN_UMV,
	SOLICITAR_A_UMV,
	RESPUESTA_A_SOLICITAR_BUFFER,
	CAMBIAR_PROCESO_ACTIVO,
	PEDIR_ETIQUETAS,
	PEDIR_INSTRUCCION
} codigos_mensajes;

void* atencionInterna(void* sinParametro) {
	char* saludoKernel = malloc(7);
	char* saludoCpu = malloc(4);
	//int socketKernel;

	//saludos_internos *saludo = malloc(sizeof(saludos_internos));
	//struct addrInfo* addrInfo;
	int socket = crearServidor(puertoUMV, logger);
	int socketKernel = aceptarConexion(socket, logger);
//	t_length *tamK;
//	int recibirMenu (socketKernel, (void*)tamK, logger);

	//escucharYCrearSocketCliente(socket, 40, socketKernel, addrInfo);
	log_debug(logger, "antes del while 1");
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
	}
	log_info(logger, "Sali del primer while 1");
	while (1) {
		//Espero conexión de cpu.
		int socketCPU = aceptarConexion(socket, logger);//Hace falta hacer listen() otra vez, como en la función de abajo?
		//Si conexión es correcta, entonces:
		cantCpu++;

		//escucharYCrearSocketCliente(socket, 40, socketCPU, addrInfo);
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
	}			//Cierra while
}			//Cierra atencionInterna

void atencionKernel(int* socketKernel) {

//	typedef struct {
//		char *msj;
//		int tamano;
//		char cantVar;
//	} t_paquete;

	int procesoActivo = 0, parametro[3], basesLogicas[3];

	char *header = malloc(16);
	int resultado, *razon = malloc(sizeof(int)), *tamanoMensaje = malloc(4);
	recv(*socketKernel, (void*) header, 16, 0);
	desempaquetar2(header, razon, tamanoMensaje, 0);
	char *mensaje = malloc(*tamanoMensaje);
	log_info(logger, "entró a atencioKernel.");

	recv(*socketKernel, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);

	switch (*razon) {

	case CREAR_SEGMENTOS_PROGRAMA:
		pthread_mutex_lock(&mutexOperacion);
		desempaquetar2(mensaje, &parametro[0], &parametro[1], &parametro[2],
				&parametro[3], 0);
		cambiarProcesoActivo(procesoActivo);
		int i;
		for (i = 0; i < 4; i++) {
			resultado = crearSegmento(parametro[i]);
			if (resultado == -1) {
				*razon = MEMORY_OVERLOAD;
				*tamanoMensaje = 0;
				destruirTodosLosSegmentos();
				break;
			}
			*razon = BASES_LOGICAS;
			*tamanoMensaje = 32;
			basesLogicas[i] = resultado;
		}
		pthread_mutex_unlock(&mutexOperacion);
		t_paquete * aSerializarHeader = (t_paquete *)serializar2(crear_nodoVar(razon, 4), crear_nodoVar(tamanoMensaje, 4), 0);
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
		pthread_mutex_lock(&mutexOperacion);
		desempaquetar2(mensaje,&parametro[0],0);
		procesoActivo = parametro[0];
		pthread_mutex_unlock(&mutexOperacion);
		break;
	}
	free(header);
	free(mensaje);
	free(tamanoMensaje);
	free(razon);

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

