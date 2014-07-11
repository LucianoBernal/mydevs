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
	int segmento_Stack;// Dirección del primer byte en la UMV del segmento de stack

	int cursor_Stack;	// Dirección del primer byte en la UMV del Contexto de Ejecución Actual
	int indice_de_Codigo;	// Dirección del primer byte en la UMV del Índice de Código
	int indice_de_Etiquetas;//Dirección del primer byte en la UMV del Índice de Etiquetas
	int program_Counter;	//Número de la próxima instrucción a ejecutar
	int tamanio_Contexto_Actual;//Cantidad de variables (locales y parámetros) del Contexto de Ejecución Actual
	int tamanio_Indice_de_Etiquetas;//Cantidad de bytes que ocupa el Índice de etiquetas
} t_PCB;


typedef struct{
	char *msj;
	char tamano;//un tamano de 255 PUEDE ser poco
	char cantVar;//pero es mejor que nada
}t_paquete;
typedef struct{
	int tamano;
	char *p_var;
}t_tamYDir;


#endif /* DEFINICIONES_H_ */
