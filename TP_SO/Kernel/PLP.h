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


extern sem_t mostarColasMutex;
extern int socketUMV;
extern t_log *logKernel;
extern sem_t  grado_Multiprogramacion;
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
extern sem_t colaReadyMutex;
extern t_queue* colaReady;
extern sem_t vacioReady;
extern sem_t colaExitVacio;
extern sem_t colaExitMutex;
extern t_queue* colaExit;



#endif /* PLPFUNCTIONS_H_ */
