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
#include <semaphore.h>

//van en el MAIN
static t_list* colaNew;
static t_list* randoms;
static int numAleatorio;
sem_t * randomMutex;
sem_init(randomMutex,0,1);
sem_t * colaNuevosMutex;
sem_init(colaNuevosMutex,0,1);
//va en el MAIN


typedef struct {
	int program_id;	//Identificador único del Programa en el sistema
	int* segmento_Codigo;// Dirección del primer byte en la UMV del segmento de código
	int* segmento_Stack;// Dirección del primer byte en la UMV del segmento de stack

	int* cursor_Stack;	// Dirección del primer byte en la UMV del Contexto de Ejecución Actual
	int* indice_de_Codigo;	// Dirección del primer byte en la UMV del Índice de Código
	int* indice_de_Etiquetas;//Dirección del primer byte en la UMV del Índice de Etiquetas
	int program_Counter;	//Número de la próxima instrucción a ejecutar
	int tamanio_Contexto_Actual;//Cantidad de variables (locales y parámetros) del Contexto de Ejecución Actual
	int tamanio_Indice_de_Etiquetas;//Cantidad de bytes que ocupa el Índice de etiquetas
} t_PCB;

typedef struct {
	t_PCB *pcb;
	int peso;
} t_new;

void asignaciones_desde_metada(t_metadata_program* metadata, t_PCB* pcb) {
	pcb->program_Counter = metadata->instruccion_inicio;
	pcb->tamanio_Indice_de_Etiquetas = metadata->etiquetas_size;
	/*pcb-> = metadata->instrucciones_size;
	 pcb-> = metadata->instrucciones_serializado;
	 pcb-> = metadata->etiquetas;
	 pcb-> = metadata->cantidad_de_funciones;
	 pcb-> = metadata->cantidad_de_etiquetas;*/
}

bool esDistinto(int* numero) {
	return (*numero != numAleatorio);
}

int estaRepetido(){
	return(!(list_all_satisfy(randoms,(void*)esDistinto)));
 }

int estaRepetido();
int generarProgramID() {
	srand(time(NULL)); // Semilla
	sem_wait(randomMutex);
	numAleatorio = rand() % 10000; //Numero Aleatorio entre 0 - 9999
	while (estaRepetido()) {
		numAleatorio = rand() % 10000;
	}
	list_add(randoms, &numAleatorio);
	int numFinal=numAleatorio;
	sem_post(randomMutex);
	return (numFinal);
}

static t_new *crear_nodoNew(t_PCB* pcb, int peso){
	t_new *nuevo = malloc( sizeof(t_new) );
	nuevo->pcb = pcb;
	nuevo->peso = peso;
	return nuevo;
}

bool menor_Peso(t_new *program1, t_new *program2) {
		return program1->peso < program2->peso;
	}


void encolar_New(t_PCB* pcb, int peso){
	t_new* nodoNuevo=crear_nodoNew(pcb,peso);
	sem_wait(colaNuevosMutex);
	list_add(colaNew,nodoNuevo);
	list_sort(colaNew,(void*) menor_Peso);
	sem_post(colaNuevosMutex);
}

int calcularPeso(t_metadata_program* metadata){
	int peso= 5*(metadata->cantidad_de_etiquetas)+ 3*(metadata->cantidad_de_funciones)+metadata->instrucciones_size ;
	return peso;
}

int generarProgramID();
void asignaciones_desde_metada(t_metadata_program*, t_PCB*);
int solicitar_Memoria(t_metadata_program*, t_PCB*);
void escribir_en_Memoria(t_metadata_program*, t_PCB*);
void encolar_New(t_PCB*, int);
void notificar_Memoria_Llena();
int calcularPeso(t_metadata_program*);

void gestionarProgramaNuevo(const char* literal) {
	randoms=list_create();//va en el MAIN
	colaNew=list_create();//va en el MAIN
	t_PCB* pcb=malloc(sizeof(t_PCB));
	t_metadata_program* metadata = metadatada_desde_literal(literal);
	pcb->program_id = generarProgramID();
	asignaciones_desde_metada(metadata,pcb);
	int peso= calcularPeso(metadata);
	if (solicitar_Memoria(metadata, pcb)== 0 /*ergo se pudo reservar memoria */) { //HABLAR CON PROJECTS LEADERS DE UMV
		escribir_en_Memoria(metadata, pcb);
		encolar_New(pcb,peso);
		} else {
		notificar_Memoria_Llena();
		free(pcb);
	}
	metadata_destruir(metadata); //OJO QUIZAS SOLO SEA EN EL ELSE REVISAR!

}

