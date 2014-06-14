/*
 * atencionInterna.c
 *
 *  Created on: 11/06/2014
 *      Author: utnso
 */
#include "atencionInterna.h"

void atencionInterna() {
	while (1) {

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

void atencionKernel(){
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
}codigos_mensajes;
}

void atencionCpu(){
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
	}codigos_mensajes;

}
