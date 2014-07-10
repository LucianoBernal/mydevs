/*
 * PLPfunctions.h
 *
 *  Created on: 10/07/2014
 *      Author: utnso
 */

#ifndef PLP_H_
#define PLP_H_

#include "Kernel.h"


typedef struct {
	t_PCB * pcb;
	int peso;
} t_new;

static sem_t * colaNuevosVacio;
static sem_t * randomMutex;
static sem_t * numABorrarMutex;
static sem_t * colaNuevosMutex;
static t_list* colaNew;
static t_list* randoms;
static t_dictionary* pidYSockets;
static int numAleatorio;
static int numABorrar;
static sem_t * PidSD_Mutex;


#endif /* PLPFUNCTIONS_H_ */
