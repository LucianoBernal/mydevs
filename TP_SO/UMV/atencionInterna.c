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

void* atencionInterna(void* sinParametro) {
	char* saludoKernel = malloc(7);
	char* saludoCpu = malloc(4);
	struct addrInfo* addrInfo;
	t_log* log= sinParametro;
	int socket = crearServidor(puertoUMV, log);
	int* socketKernel = malloc(4);
	aceptarConexion()
	//escucharYCrearSocketCliente(socket, 40, socketKernel, addrInfo);
	recv(socketKernel, (void*) saludoKernel, 30, 0);
	if (strncmp(saludoKernel, "Kernel", 6)) {
		pthread_create(&hiloKernel, NULL, (void *) atencionKernel,
				(void*) socketKernel);
	} else {
		close(socketKernel);
	}
	while (1) {
		int* socketCliente = malloc(4);
		escucharYCrearSocketCliente(socket, 40, socketCliente, addrInfo);
		recv(socketCliente, (void*) saludoCpu, 30, 0);
		if (strncmp(saludoCpu, "CPU", 3)) {
			pthread_create(&hiloCpu, NULL, (void *) atencionCpu,
					(void*) socketCliente);
			cantCpu++;
		} else {
			close(socketCliente);
		}

		//Creo hilo que atiende al kernel.
		if (pthread_create(&hiloKernel, NULL, (void *) atencionKernel, NULL )) {
			//ERROR. Acá van cosas con log.
		} else {
			//Se creó correctamente.
		}
		//Espero conexión de cpu.
		//Si conexión es correcta, entonces:
		cantCpu++;
		//Creo hilo que atiende a las cpu's.
		if (pthread_create(&hiloCpu, NULL, (void *) atencionCpu, NULL )) {
			//ERROR. Acá van cosas con log.
		} else {
			//Se creó correctamente.
		}

	}

}

void atencionKernel(int* socketKernel) {

	typedef struct {
		char *msj;
		int tamano;
		char cantVar;
	} t_paquete;

	int pid;
	int base, offset, tamano;
/*
	t_paquete* header; //TODO colocar t_paquete en definiciones.h urgente!!
	t_paquete* paquete;
*/
	char *header, *mensaje;
	printf( "Se conectó el Kernel y se creó un hilo que atiende su ejecución :D");
	int *razon=malloc(sizeof(int)), *tamanoMensaje=malloc(4);
	recv(*socketKernel, (void*) header, 16, MSG_WAITALL);
	desempaquetar2(header, razon, tamanoMensaje, 0);

	recv(*socketKernel, (void*) mensaje, *tamanoMensaje, MSG_WAITALL);

	switch (*razon) {
	case CREAR_SEGMENTO:

		desempaquetar2(mensaje, &pid, 0);
		crearSegmento(pid);
		break;
	case DESTRUIR_SEGMENTOS:
		destruirTodosLosSegmentos();
		break;
	case SOLICITAR_A_UMV:

		desempaquetar2(paquete->msj, base, offset, tamano, 0);
		solicitarBytes( base, offset, tamano);
		break;
	}

}
void atencionCpu() {
	printf("Se conectó una CPU y se creó un hilo que la atiende :D :D");
}

