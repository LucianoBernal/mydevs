/*
 * PLP.C
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>

static t_list* randoms; //van en el MAIN
static int numAleatorio; //va en el MAIN

typedef struct {
	int program_id;	//Identificador único del Programa en el sistema
	void* segmento_Codigo;// Dirección del primer byte en la UMV del segmento de código
	void* segmento_Stack;// Dirección del primer byte en la UMV del segmento de stack

	void* cursor_Stack;	// Dirección del primer byte en la UMV del Contexto de Ejecución Actual
	void* indice_de_Codigo;	// Dirección del primer byte en la UMV del Índice de Código
	void* indice_de_Etiquetas;//Dirección del primer byte en la UMV del Índice de Etiquetas
	int program_Counter;	//Número de la próxima instrucción a ejecutar
	int tamanio_Contexto_Actual;//Cantidad de variables (locales y parámetros) del Contexto de Ejecución Actual
	int tamanio_Indice_de_Etiquetas;//Cantidad de bytes que ocupa el Índice de etiquetas
} t_PCB;

void asignaciones_desde_metada(t_metadata_program* metadata, t_PCB* pcb) {
	pcb->program_Counter = metadata->instruccion_inicio;
	pcb->tamanio_Indice_de_Etiquetas = metadata->etiquetas_size;
	/*pcb-> = metadata->instrucciones_size;
	 pcb-> = metadata->instrucciones_serializado;
	 pcb-> = metadata->etiquetas;
	 pcb-> = metadata->cantidad_de_funciones:
	 pcb-> = metadata->cantidad_de_etiquetas;*/
}

bool esDistinto(int* numero) {
	return (*numero != numAleatorio);
}

int estaRepetido(){
	randoms=list_create();
	return(!(list_all_satisfy(randoms,(void*)esDistinto)));
 }

int estaRepetido();
int generarProgramID() {
	srand(time(NULL)); // Semilla
	numAleatorio = rand() % 10000; //Numero Aleatorio entre 0 - 9999
	while (estaRepetido()) {
		numAleatorio = rand() % 10000;
	}
	list_add(randoms, &numAleatorio);
	return (numAleatorio);
}

int generarProgramID();
void asignaciones_desde_metada(t_metadata_program*, t_PCB*);
int solicitar_Memoria(t_metadata_program*, t_PCB*);
void escribir_en_Memoria(t_metadata_program*, t_PCB*);
void encolar_New(t_PCB*);
void notificar_Memoria_Llena;

void gestionarProgramaNuevo(const char* literal) {
	t_PCB* pcb;
	t_metadata_program* metadata = metadatada_desde_literal(literal);
	pcb->program_id = generarProgramID();
	asignaciones_desde_metada(metadata,pcb);
	if (solicitar_Memoria(metadata, pcb)== 0 /*ergo se pudo reservar memoria */) { //HABLAR CON PROJECTS LEADERS DE UMV
		escribir_en_Memoria(metadata, pcb);
		encolar_New(pcb);
	} else {
		notificar_Memoria_Llena();
	}
}

