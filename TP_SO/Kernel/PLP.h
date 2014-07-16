/*
 * PLPfunctions.h
 *
 *  Created on: 10/07/2014
 *      Author: utnso
 */

#ifndef PLP_H_
#define PLP_H_

#include "Kernel.h"
#include "multiplexorScripts_interfaz.h"

sem_t mutexProcesoActivo;
sem_t  colaNuevosVacio;
sem_t  randomMutex;
sem_t  numABorrarMutex;
sem_t  colaNuevosMutex;
t_list* colaNew;
t_list* randoms;
t_dictionary* pidYSockets;
int numAleatorio;
int numABorrar;
sem_t  PidSD_Mutex;


#endif /* PLPFUNCTIONS_H_ */
