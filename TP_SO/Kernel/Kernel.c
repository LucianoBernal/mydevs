/*
 * Kernel.c
 *
 *  Created on: 27/05/2014
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include "PLP.h"



void kernel_main(){
	//Leer archivo Config
	static t_queue* colaReady;
	colaReady=list_create();
	static sem_t * colaReadyMutex;
	sem_init(colaReadyMutex,0,1);
	static sem_t * vacioReady;
	sem_init(vacioReady,0,0);
}

void* plp(void* parametro){
	colaNew=list_create();
	randoms=list_create();
	sem_init(randomMutex,0,1);
	sem_init(numABorrarMutex,0,1);
	sem_init(colaNuevoSemaforo,0,0);
	sem_init(colaNuevosMutex,0,1);
}
