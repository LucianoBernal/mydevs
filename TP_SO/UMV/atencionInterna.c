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
	Kernel=1,
	CPU=2
}saludos_internos;

typedef enum {
	CONFIRMACION,
	SEGMENTATION_FAULT,
	MEMORY_OVERLOAD,
	MOSTRAR_VALOR,
	MOSTRAR_TEXTO,
	CREAR_SEGMENTO,
	CREAR_SEGMENTOS_PROGRAMA,
	DESTRUIR_SEGMENTOS,
	ESCRIBIR_EN_UMV,
	ESCRIBIR_EN_UMV_OFFSET_CERO,
	SOLICITAR_A_UMV,
	PEDIR_ETIQUETAS,
	PEDIR_INSTRUCCION
} codigos_mensajes;

//void* atencionInterna(void* sinParametro) {
//	//char* saludoKernel = malloc(7);
//	saludos_internos *saludo = malloc(sizeof(saludos_internos));
//	//char* saludoCpu = malloc(4);
//	struct addrInfo* addrInfo;
//	t_log* log= sinParametro;
//	int socket = crearServidor(puertoUMV, log);
//	int socketKernel;
//	socketToby=aceptarConexion(socket,log);//El 1er parametro es socket o socketKernel? y el segundo es log o logger?
//	//escucharYCrearSocketCliente(socket, 40, socketKernel, addrInfo);
//	recv(socketToby/*socketKernel*/, (void*) saludo, 30, 0);
//	/*if (strncmp(saludoKernel, "Kernel", 6))*/
//	if(*saludo == Kernel){
//		pthread_create(&hiloKernel, NULL, (void *) atencionKernel,
//				(void*) socketKernel);
//	} else {
//		close(socketKernel);
//	}
//	while (1) {
//		int socketCliente;
//		escucharYCrearSocketCliente(socket, 40, socketCliente, addrInfo);
//		recv(socketCliente, (void*) saludo, 30, 0);
//		/*if (strncmp(saludoCpu, "CPU", 3))*/
//		if (*saludo==CPU){
//			pthread_create(&hiloCpu, NULL, (void *) atencionCpu,
//					(void*) socketCliente);
//			cantCpu++;
//		} else {
//			close(socketCliente);
//		}
//
//		//Creo hilo que atiende al kernel.
//		if (pthread_create(&hiloKernel, NULL, (void *) atencionKernel, NULL )) {
//			log_error(logger,"El hilo del Kernel no se creó correctamente.");
//		} else {
//			log_info(logger, "El hilo del Kernel se creó correctamente.");
//		}
//		//Espero conexión de cpu.
//		//Si conexión es correcta, entonces:
//		cantCpu++;
//		//Creo hilo que atiende a las cpu's.
//		if (pthread_create(&hiloCpu, NULL, (void *) atencionCpu, NULL )) {
//			log_error(logger,"El hilo de CPU no se creó correctamente.");
//		} else {
//			log_info(logger, "El hilo de CPU se creó correctamente.");
//		}
//
//	}
//
//}

void atencionKernel(int* socketKernel) {

	typedef struct {
		char *msj;
		int tamano;
		char cantVar;
	} t_paquete;

	//int pid;
	int base, offset, tamano;

	char *header=NULL, *mensaje=NULL;
	printf( "Se conectó el Kernel y se creó un hilo que atiende su ejecución :D");
	int *razon=malloc(sizeof(int)), *tamanoMensaje=malloc(4);
	recv(*socketKernel, (void*) header, 16, MSG_WAITALL);
	desempaquetar2(header, razon, tamanoMensaje, 0);

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
		solicitarBytes( base, offset, tamano);
		break;
	}

}
void atencionCpu() {
	printf("Se conectó una CPU y se creó un hilo que la atiende :D :D");
}

