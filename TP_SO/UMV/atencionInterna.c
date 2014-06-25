/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
/*#include "atencionInterna.h"

void atencionInterna() {
	char* saludoKernel= malloc(7);
	char* saludoCpu=malloc(4);
	struct addrInfo* addrInfo;
	obtenerAddrInfoLocalHost(addrInfo, puertoUMV);
	int socket = crearSocket(addrInfo);
	bindearSocket(socket, addrInfo);
	int* socketKernel = malloc(4);
	escucharYCrearSocketCliente(socket, 40, socketKernel, addrInfo);
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
		/*
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
		 */
/*
	}

}

void atencionKernel() {
	printf("Se conectó el Kernel y se creó un hilo que atiende su ejecución :D");
	/*typedef enum {
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
*/
/*}
void atencionCpu() {
	printf("Se conectó una CPU y se creó un hilo que la atiende :D :D");
}
*/
