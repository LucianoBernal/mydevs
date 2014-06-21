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
#include <commons/collections/dictionary.h>
#include <semaphore.h>
#include <sys/socket.h>

static t_list* colaNew;
static t_list* randoms;
static t_dictionary* pidYSockets=dictionary_create();
static int numAleatorio;
static int numABorrar;
static sem_t * PidSD_Mutex = NULL;
sem_init(PidSD_Mutex,0,1);
static sem_t * colaNuevosVacio = NULL;
static sem_t * randomMutex = NULL;
static sem_t * numABorrarMutex = NULL;
static sem_t * colaNuevosMutex = NULL;

typedef enum {
	SEGMENTATION_FAULT,
	MEMORY_OVERLOAD,
	CREAR_SEGMENTO,
	CREAR_SEGMENTOS_PROGRAMA,
	DESTRUIR_SEGMENTOS,
	ESCRIBIR_EN_UMV,
	ESCRIBIR_EN_UMV_OFFSET_CERO,
	ENVIAR_PCB,
	SOLICITAR_A_UMV,
	PEDIR_ETIQUETAS,
	PEDIR_INSTRUCCION
};
#define TAMANO_CABECERA 7
int socketUMV; //Agregada por pato, deberias crear el socket en algun momento.
int tamanoStack; //Deberia leerlo desde config
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
	pthread_join(threadColaNew, NULL );

}

//int generarProgramID();
//void asignaciones_desde_metada(t_metadata_program*, t_PCB*);
int solicitar_Memoria(t_metadata_program*, t_PCB*);
void escribir_en_Memoria(t_metadata_program*, t_PCB*);
//void encolar_New(t_PCB*, int);
void notificar_Memoria_Llena();
//int calcularPeso(t_metadata_program*);
//void liberar_numero(int pid);
int solicitar_Memoria(t_metadata_program *metadata, t_PCB *pcb,
		const char* literal) {
	int *codigoRespuesta = malloc(sizeof(int));
	char *tamanoRespuesta = malloc(sizeof(char));
	//Ya lo agregue a definiciones.h, sigue tirando error
	t_paquete *mensaje = serializar2(strlen(literal), metadata->etiquetas_size,
			tamanoStack, metadata->instrucciones_size);
	int *razon = malloc(sizeof(int));
	*razon = CREAR_SEGMENTOS_PROGRAMA;
	t_paquete *header = serializar2(crear_nodoVar(&mensaje->tamano, 1),
			crear_nodoVar(razon, sizeof(int)), 0);
	send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
	send(socketUMV, (void*) mensaje, mensaje->tamano, 0);
	char *hRespuesta = malloc(TAMANO_CABECERA);
	recv(socketUMV, (void*) hRespuesta, TAMANO_CABECERA, MSG_WAITALL);
	desempaquetar2(hRespuesta, tamanoRespuesta, codigoRespuesta, 0);
	if (codigoRespuesta) {
		char *msjRespuesta = malloc(tamanoRespuesta);
		recv(socketUMV, (void*) msjRespuesta, tamanoRespuesta, MSG_WAITALL);
		desempaquetar2(msjRespuesta, pcb->segmento_Codigo,
				pcb->indice_de_Etiquetas, pcb->segmento_Stack,
				pcb->indice_de_Codigo, 0);
	} else {
		printf("No habia espacio en memoria");
	}
	return *codigoRespuesta;
}

void escribir_en_Memoria(t_metadata_program * metadata, t_PCB *pcb,
		const char *literal) {
	int *razon = malloc(sizeof(int));
	*razon = ESCRIBIR_EN_UMV_OFFSET_CERO;
	char error = 1;
	//Segmento codigo
	t_paquete *paquete = serializar2(crear_nodoVar(&(pcb->program_id), 4),
			crear_nodoVar((pcb->segmento_Codigo), 4),
			crear_nodoVar(literal, strlen(literal)), 0);
	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
			crear_nodoVar(razon, sizeof(int)), 0);
	if (error)
		error = send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
	if (error)
		error = send(socketUMV, (void*) paquete, paquete->tamano, 0);
	//indice etiquetas
	t_paquete *paquete = serializar2(crear_nodoVar(&(pcb->program_id), 4),
			crear_nodoVar((pcb->indice_de_Etiquetas), 4),
			crear_nodoVar(metadata->etiquetas, metadata->etiquetas_size), 0);
	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
			crear_nodoVar(razon, sizeof(int)), 0);
	if (error)
		error = send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
	if (error)
		error = send(socketUMV, (void*) paquete, paquete->tamano, 0);
	//indice codigo
	t_paquete *paquete = serializar2(crear_nodoVar(&(pcb->program_id), 4),
			crear_nodoVar((pcb->indice_de_Codigo), 4),
			crear_nodoVar(metadata->instrucciones_serializado,
					metadata->instrucciones_size), 0);
	t_paquete *header = serializar2(crear_nodoVar(&(paquete->tamano), 4),
			crear_nodoVar(razon, sizeof(int)), 0);
	if (error)
		error = send(socketUMV, (void*) header, TAMANO_CABECERA, 0);
	if (error)
		error = send(socketUMV, (void*) paquete, paquete->tamano, 0);
	if (!error)
		printf("Alguno de los sends fallo, noob");
}

agregar_En_Diccionario(int pid, int sd){
	sem_wait(PidSD_Mutex);
	dictionary_put(pidYSockets,(char*)&pid,&sd); //FIXME
	sem_post(PidSD_Mutex);

}

void gestionarProgramaNuevo(const char* literal,int sd) { // UN HILO
	t_PCB* pcb = malloc(sizeof(t_PCB));
	t_metadata_program* metadata = metadatada_desde_literal(literal);
	pcb->program_id = generarProgramID();
	asignaciones_desde_metada(metadata, pcb);
	int peso = calcularPeso(metadata);
	if (solicitar_Memoria(metadata, pcb, literal)
			== 0 /*ergo se pudo reservar memoria */) { //HABLAR CON PROJECTS LEADERS DE UMV
		escribir_en_Memoria(metadata, pcb, literal);
		encolar_New(pcb, peso);
		agregar_En_Diccionario(pcb->program_id,sd);
	} else {
		notificar_Memoria_Llena();
		free(pcb);
		liberar_numero(pcb->program_id);
	}
	metadata_destruir(metadata); //OJO QUIZAS SOLO SEA EN EL ELSE REVISAR!
}
//void encolar_en_Ready(t_PCB*);
void deNewAReady(int sinParametro) { // OTRO HILO
	while (1) {
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
