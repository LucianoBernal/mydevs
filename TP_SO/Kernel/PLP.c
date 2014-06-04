/*
 * PLP.C
 *
 *  Created on: 18/05/2014
 *      Author: utnso
 */

#include "PLP.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

static t_list* colaNew;
static t_list* randoms;
static int numAleatorio;
static int numABorrar;
static sem_t * colaNuevosVacio = NULL;
static sem_t * randomMutex = NULL;
static sem_t * numABorrarMutex = NULL;
static sem_t * colaNuevosMutex = NULL;

void asignaciones_desde_metada(t_metadata_program* metadata, t_PCB* pcb) {
	pcb->program_Counter = metadata->instruccion_inicio;
	pcb->tamanio_Indice_de_Etiquetas = metadata->etiquetas_size;
	/*pcb-> = metadata->instrucciones_size;
	 pcb-> = metadata->instrucciones_serializado;
	 pcb-> = metadata->etiquetas;
	 pcb-> = metadata->cantidad_de_funciones;
	 pcb-> = metadata->cantidad_de_etiquetas;*/
}

bool esDistintoANumAleatorio(int* numero) {
	return (*numero != numAleatorio);
}

int estaRepetido() {
	return (!(list_all_satisfy(randoms, (void*) esDistintoANumAleatorio)));
}

//int estaRepetido();
int generarProgramID() {
	srand(time(NULL )); // Semilla
	sem_wait(randomMutex);
	numAleatorio = rand() % 10000; //Numero Aleatorio entre 0 - 9999
	while (estaRepetido()) {
		numAleatorio = rand() % 10000;
	}
	list_add(randoms, &numAleatorio);
	int numFinal = numAleatorio;
	sem_post(randomMutex);
	return (numFinal);
}

static t_new *crear_nodoNew(t_PCB* pcb, int peso) {
	t_new *nuevo = malloc(sizeof(t_new));
	nuevo->pcb = pcb;
	nuevo->peso = peso;
	return nuevo;
}

bool menor_Peso(t_new *program1, t_new *program2) {
	return program1->peso < program2->peso;
}

void encolar_New(t_PCB* pcb, int peso) {
	t_new* nodoNuevo = crear_nodoNew(pcb, peso);
	sem_wait(colaNuevosMutex);
	list_add(colaNew, nodoNuevo);
	list_sort(colaNew, (void*) menor_Peso);
	sem_post(colaNuevosMutex);
	sem_post(colaNuevosVacio);

}

int calcularPeso(t_metadata_program* metadata) {
	int peso = 5 * (metadata->cantidad_de_etiquetas)
			+ 3 * (metadata->cantidad_de_funciones)
			+ metadata->instrucciones_size;
	return peso;
}

bool esIgualANumABorrar(int* numero) {
	return (*numero == numABorrar);
}

void liberar_numero(int pid) {
	sem_wait(numABorrarMutex);
	numABorrar = pid;
	list_remove_by_condition(randoms, (void*) esIgualANumABorrar);
	sem_post(numABorrarMutex);

}

void lanzarHiloColaNewAReady() {
	pthread_t threadColaNew;
	int iretColaNew;
	int sinParametro;
	iretColaNew = pthread_create(&threadColaNew, NULL, deNewAReady,
			(void*) sinParametro);
	if (iretColaNew) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n",
				iretColaNew);

	}
	pthread_join( threadColaNew, NULL);

}

//int generarProgramID();
//void asignaciones_desde_metada(t_metadata_program*, t_PCB*);
int solicitar_Memoria(t_metadata_program*, t_PCB*);
void escribir_en_Memoria(t_metadata_program*, t_PCB*);
//void encolar_New(t_PCB*, int);
void notificar_Memoria_Llena();
//int calcularPeso(t_metadata_program*);
//void liberar_numero(int pid);

void gestionarProgramaNuevo(const char* literal) { // UN HILO
	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata = metadatada_desde_literal(literal);
	pcb->program_id = generarProgramID();
	asignaciones_desde_metada(metadata, pcb);
	int peso = calcularPeso(metadata);
	if (solicitar_Memoria(metadata, pcb)
			== 0 /*ergo se pudo reservar memoria */) { //HABLAR CON PROJECTS LEADERS DE UMV
		escribir_en_Memoria(metadata, pcb);
		encolar_New(pcb, peso);
		} else {
		notificar_Memoria_Llena();
		free(pcb);
		liberar_numero(pcb->program_id);
	}
	metadata_destruir(metadata); //OJO QUIZAS SOLO SEA EN EL ELSE REVISAR!

}
//void encolar_en_Ready(t_PCB*);
void deNewAReady(int sinParametro) { // OTRO HILO
	while(1){
	sem_wait(colaNuevosVacio);
	sem_wait(grado_Multiprogramacion);
	t_new* elementoSacado;
	sem_wait(colaNuevosMutex);
	elementoSacado = list_remove(colaNew, 0);
	sem_post(colaNuevosMutex);
	t_PCB* pcb_Ready;
	pcb_Ready = elementoSacado->pcb;
	encolar_en_Ready(pcb_Ready);
	free(elementoSacado);
	}
}

void encolar_en_Ready(t_PCB* pcb) {
	sem_wait(colaReadyMutex);
	queue_push(colaReady, pcb);
	sem_post(colaReadyMutex);
	sem_post(vacioReady);
}
