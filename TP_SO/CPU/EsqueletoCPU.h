/*
 * EsqueletoCPU.h
 *
 *  Created on: 18/07/2014
 *      Author: utnso
 */

#ifndef ESQUELETOCPU_H_
#define ESQUELETOCPU_H_

#include <commons/log.h>
#include <signal.h>

int socketUMV, socketKernel;
int sigusr1_activado;
t_log* logs;
t_dictionary* diccionarioDeVariables;
int proceso_terminado;
int proceso_bloqueado;
int quantumDeKernel;
int retardo;

#endif /* ESQUELETOCPU_H_ */
