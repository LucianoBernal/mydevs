/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "atencionInterna.h"
#include "atencioninterna_interfaz.h"
#include <sockets/Serializacion.h>

typedef enum {
	Kernel = 1, CPU = 2
} saludos_internos;

typedef enum {
	CONFIRMACION,
	SEGMENTATION_FAULT,
	MEMORY_OVERLOAD,
	MOSTRAR_VALOR,
	MOSTRAR_TEXTO,
	//CREAR_SEGMENTO,
	CREAR_SEGMENTOS_PROGRAMA,
	DESTRUIR_SEGMENTOS,
	ESCRIBIR_EN_UMV,
	ESCRIBIR_EN_UMV_OFFSET_CERO,
	SOLICITAR_A_UMV,
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

			//saludo = tamK->menu;//FIXME homogeneizar codigos_mensajes y t_menu y saludos_internos
			//if(*saludo == Kernel){
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

//		t_length *tamC;
//		saludo = tamC->menu;//TODO ver bien lo del saludo!!!
			//int recibirMenu (socketCPU,(void*)tamC, logger);
			//if (*saludo==CPU){
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

	typedef struct {
		char *msj;
		int tamano;
		char cantVar;
	} t_paquete;

	//int pid;
	int base, offset, tamano;

	char *header = malloc(16), *mensaje = malloc(30);
	int *razon = malloc(sizeof(int)), *tamanoMensaje = malloc(4);
	recv(*socketKernel, (void*) header, 16, 0);
	//desempaquetar2(header, razon, tamanoMensaje, 0);
	log_info(logger, "entró a atencioKernel.");

	recv(*socketKernel, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);

	switch (*razon) {
	case CREAR_SEGMENTO:

		desempaquetar2(mensaje, &tamano, 0);
		crearSegmento(tamano);
		break;
	case DESTRUIR_SEGMENTOS:
		destruirTodosLosSegmentos();
		break;
	case SOLICITAR_A_UMV:

		desempaquetar2(mensaje, &base, &offset, &tamano, 0);
		solicitarBytes(base, offset, tamano);
		break;
	}

}
void atencionCpu() {
	log_info(logger, "entro a atencionCpu");
	//printf("Se conectó una CPU y se creó un hilo que la atiende :D :D");
}

