/*
 * Definiciones.h
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */

#ifndef DEFINICIONES_H_
#define DEFINICIONES_H_

typedef struct {
	int program_id;	//Identificador único del Programa en el sistema
	int segmento_Codigo;// Dirección del primer byte en la UMV del segmento de código
	int segmento_Stack;	// Dirección del primer byte en la UMV del segmento de stack

	int cursor_Stack;// Dirección del primer byte en la UMV del Contexto de Ejecución Actual
	int indice_de_Codigo;// Dirección del primer byte en la UMV del Índice de Código
	int indice_de_Etiquetas;//Dirección del primer byte en la UMV del Índice de Etiquetas
	int program_Counter;	//Número de la próxima instrucción a ejecutar
	int tamanio_Contexto_Actual;//Cantidad de variables (locales y parámetros) del Contexto de Ejecución Actual
	int tamanio_Indice_de_Etiquetas;//Cantidad de bytes que ocupa el Índice de etiquetas
} t_PCB;

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
	CREAR_PROCESO_NUEVO,
	PEDIR_INSTRUCCION,
	ENVIAR_PCB,
	ESCRIBIR_EN_UMV_OFFSET_CERO,
	OBTENER_VALOR_COMPARTIDA,
	ASIGNAR_VALOR_COMPARTIDA,
	IMPRIMIR,
	IMPRIMIR_TEXTO,
	SALIO_POR_IO,
	WAIT,
	SIGNAL,
	DESALOJAR_PROGRAMA,
	NUEVO_PROGRAMA,
	SALIDA_POR_QUANTUM,
	SALIDA_POR_BLOQUEO,
	SALIDA_NORMAL,
	SALIDA_POR_SEMAFORO,
	SIG_INT,
	SIGURS_1,
	GRABAR_VALOR,
	OBTENER_VALOR
} codigos_Mensajes;

#endif /* DEFINICIONES_H_ */
